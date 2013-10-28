#include <errno.h>
#include "gtest/gtest.h"
#include "test/librados/test.h"
#include "include/rados/librados.h"

#include "include/types.h"
#include "cls/llvm/cls_llvm_client.h"

#include "test_lib.h"
#include "test/cls_llvm/cls_llvm_test.pb.h"

static string test_lib;

// TODO: oleiman - notice that some of these tests are less meaningful
//    than desired on account of not having chosen a serialization
//    format. E.g. I'd like my stat_ret function to return some
//    structured data. Protobuf has been a hassle, considering msgpack

class ClsLLVM : public ::testing::Test {
  
  protected:
    static void SetUpTestCase() {
      pool_name = get_temp_pool_name();
      ASSERT_EQ("", create_one_pool_pp(pool_name, rados));
      ASSERT_EQ(0, rados.ioctx_create(pool_name.c_str(), ioctx));

      /* auto-generated from test_script.lua */
      test_lib.assign(string(cls_llvm_test_lib));
    }

    static void TearDownTestCase() {
      ioctx.close();
      ASSERT_EQ(0, destroy_one_pool_pp(pool_name, rados));
    }

    virtual void SetUp() {

      /* Grab test names to build unique objects */
      const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();

      /* Create unique string using test/testname/pid */
      std::stringstream ss_oid;
      ss_oid << test_info->test_case_name() << "_" <<
        test_info->name() << "_" << getpid();

      /* Unique object for test to use */
      oid = ss_oid.str();

      bufferptr bp(cls_llvm_test_lib, cls_llvm_test_lib_size);
      bitcode.push_back(bp);

    }
  
    static librados::Rados rados;
    static librados::IoCtx ioctx;
    static string pool_name;
    static string test_script;

    bufferlist bitcode;
    string oid;
    bufferlist reply_output;

};

librados::Rados ClsLLVM::rados;
librados::IoCtx ClsLLVM::ioctx;
string ClsLLVM::pool_name;
string ClsLLVM::test_script;

TEST_F(ClsLLVM, ReturnStringArg)
{
  string result;
  vector<string> rlog;
  int ret;

  bufferlist input, output;

  string written = "Hello, World!";
  bufferptr argp(written.c_str(), written.length());
  input.push_back(argp);

  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid, bitcode, "retStr", input, output, &rlog));

  bufferlist::iterator iter = output.begin();
  ::decode(result, iter);
  
  ASSERT_EQ(0, ret);
  
  ASSERT_EQ("Hello, World!", result);
  
  ASSERT_EQ("return value: 0", rlog[0]);
}

TEST_F(ClsLLVM, Write)
{
  vector<string> rlog;
  bufferlist inbl;
  string written = "Hello World";
  ::encode(written, inbl);

  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid, bitcode, "Write", inbl, reply_output, &rlog));

  /* have Lua read out of the object */
  uint64_t size;
  bufferlist outbl;
  ASSERT_EQ(0, ioctx.stat(oid, &size, NULL));
  ASSERT_EQ(size, (uint64_t)ioctx.read(oid, outbl, size, 0) );

  /* compare what Lua read to what we wrote */
  string read;
  ::decode(read, outbl);
  ASSERT_EQ(read, written);
}

TEST_F(ClsLLVM, Create) 
{
  /* create works */
  vector<string> rlog;
  bufferlist inbl;

  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid, bitcode, "create_c", inbl, reply_output, &rlog));

  /* exclusive works */
  ASSERT_EQ(-EEXIST, cls_llvm_exec(ioctx, oid, bitcode, "create_c", inbl, reply_output, &rlog));
  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid, bitcode, "create_cne", inbl, reply_output, &rlog));
}

TEST_F(ClsLLVM, Stat) 
{
  /* build object and stat */
  char buf[1024];
  vector<string> rlog;
  bufferlist bl, inbl;

  bl.append(buf, sizeof(buf));
  ASSERT_EQ(0, ioctx.write_full(oid, bl));
  uint64_t size;
  time_t mtime;
  ASSERT_EQ(0, ioctx.stat(oid, &size, &mtime));

  /* test stat success */
  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid, bitcode, "stat_ret", inbl, reply_output, &rlog));

  // ClsLlvmTest__StatRet sr = CLS_LLVM_TEST__STAT_RET__INIT;
  // sr.size = 23;
  // sr.mtime = 42;
  // size_t len = cls_llvm_test__stat_ret__get_packed_size(&sr);
  // void *secondbuf = malloc(len);
  // cls_llvm_test__stat_ret__pack(&sr, (uint8_t *) secondbuf);

  /* TODO: unpack protobuf here */
  // cls_llvm_test::StatRet stat_result;
  // string reply_data;
  // reply_data.assign(string(reply_output.c_str()));
  
  // stat_result.ParseFromString(reply_data);

  /* test object dne */
  ASSERT_EQ(-ENOENT, cls_llvm_exec(ioctx, "dne", bitcode, "stat_dne", inbl, reply_output, &rlog));
}

TEST_F(ClsLLVM, Read) {
  /* put data into object */
  string msg = "This is a test message";
  vector<string> rlog;
  bufferlist bl, outbl;
  bl.append(msg.c_str(), msg.size());
  ASSERT_EQ(0, ioctx.write_full(oid, bl));
  
  ASSERT_EQ(msg.length(), (size_t) cls_llvm_exec(ioctx, oid, bitcode, "Read", bl, outbl, &rlog));

  /* check return */
  string ret_val;
  bufferlist::iterator iter = outbl.begin();
  ::decode(ret_val, iter);
  ASSERT_EQ("This is a test message", ret_val);
}

TEST_F(ClsLLVM, MapGetVal) {
  /* write some data into a key */
  string msg = "This is a test message for the map";
  vector<string> rlog;
  bufferlist bl, outbl;
  bufferlist orig_val;
  orig_val.append(msg.c_str(), msg.size());
  map<string, bufferlist> orig_map;
  orig_map["foo"] = orig_val;
  ASSERT_EQ(0, ioctx.omap_set(oid, orig_map));

  /* now compare to what we put it */
  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid, bitcode, "map_get_val_foo", bl, outbl, &rlog));

  /* check return */
  string ret_val;
  bufferlist::iterator iter = outbl.begin();
  ::decode(ret_val, iter);
  ASSERT_EQ(ret_val, msg);

  /* error case */
  // ASSERT_EQ(-ENOENT, cls_llvm_exec(ioctx, oid, bitcode, "map_get_val_dne", bl, outbl, &rlog));
}

TEST_F(ClsLLVM, MapSetVal) {
  /* build some input value */
  bufferlist orig_val, outbl;
  vector<string> rlog;
  ::encode("this is the original value yay", orig_val);

  /* have the lua script stuff the data into a map value */
  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid, bitcode, "map_set_val_foo", orig_val, outbl, &rlog));

  /* grap the key now and compare to orig */
  map<string, bufferlist> out_map;
  set<string> out_keys;
  out_keys.insert("foo");
  ASSERT_EQ(0, ioctx.omap_get_vals_by_keys(oid, out_keys, &out_map));
  bufferlist val_bl = out_map["foo"];
  string out_val;
  ::decode(out_val, val_bl);
  ASSERT_EQ(out_val, "this is the original value yay");
}

TEST_F(ClsLLVM, MapClear) {
  /* write some data into a key */
  string msg = "This is a test message";
  bufferlist val;
  val.append(msg.c_str(), msg.size());
  map<string, bufferlist> map;
  map["foo"] = val;
  ASSERT_EQ(0, ioctx.omap_set(oid, map));

  /* test we can get it back out */
  set<string> keys;
  keys.insert("foo");
  map.clear();
  ASSERT_EQ(0, (int)map.count("foo"));
  ASSERT_EQ(0, ioctx.omap_get_vals_by_keys(oid, keys, &map));
  ASSERT_EQ(1, (int)map.count("foo"));

  /* now clear it */
  bufferlist inbl, outbl;
  vector<string> rlog;

  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid, bitcode, "map_clear", inbl, outbl, &rlog));
  
  /* test that the map we get back is empty now */
  map.clear();
  ASSERT_EQ(0, (int)map.count("foo"));
  ASSERT_EQ(0, ioctx.omap_get_vals_by_keys(oid, keys, &map));
  ASSERT_EQ(0, (int)map.count("foo"));
}
