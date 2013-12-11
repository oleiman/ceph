#include <errno.h>
#include <msgpack.h>

#include "gtest/gtest.h"
#include "test/librados/test.h"
#include "include/rados/librados.h"

#include "include/types.h"
#include "cls/llvm/cls_llvm_client.h"
#include "cls_llvm_test.pb-c.h"

#include "test_lib.h"
#include "ceph.h"

class ClsLLVM : public ::testing::Test {
  
  protected:
    static void SetUpTestCase() {
      pool_name = get_temp_pool_name();
      ASSERT_EQ("", create_one_pool_pp(pool_name, rados));
      ASSERT_EQ(0, rados.ioctx_create(pool_name.c_str(), ioctx));
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
      std::stringstream ss_oid1;
      ss_oid1 << test_info->test_case_name() << "_" <<
        test_info->name() << "_" << getpid() << 1;

      /* Unique object for test to use */
      oid1 = ss_oid1.str();

      std::stringstream ss_oid2;
      ss_oid2 << test_info->test_case_name() << "_" <<
        test_info->name() << "_" << getpid() << 2;

      oid2 = ss_oid2.str();

      bufferptr bp(cls_llvm_test_lib, cls_llvm_test_lib_size);
      bitcode.push_back(bp);

    }
  
    static librados::Rados rados;
    static librados::IoCtx ioctx;
    static string pool_name;
    static string test_script;

    bufferlist bitcode;
    string oid1;
    string oid2;
  
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

  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid1, bitcode, "retStr", input, output, &rlog));

  result.assign(string(output.c_str()));
  
  ASSERT_EQ(0, ret);
  
  ASSERT_EQ("Hello, World!", result);
  
  ASSERT_EQ("return value: 0", rlog[0]);
}

TEST_F(ClsLLVM, Write)
{
  vector<string> rlog;
  bufferlist inbl;
  string written = "Hello World";
  inbl.append(written.data(), written.length());
  
  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid1, bitcode, "Write", inbl, reply_output, &rlog));

  /* read out of the object */
  uint64_t size;
  bufferlist outbl;
  ASSERT_EQ(0, ioctx.stat(oid1, &size, NULL));
  ASSERT_EQ(size, (uint64_t)ioctx.read(oid1, outbl, size, 0) );

  // compare what was read to what we wrote
  string read;
  outbl.copy(0, outbl.length(), read);
  
  ASSERT_EQ(read, written);
}

TEST_F(ClsLLVM, Create) 
{
  /* create works */
  vector<string> rlog;
  bufferlist inbl;

  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid1, bitcode, "create_c", inbl, reply_output, &rlog));

  /* exclusive works */
  ASSERT_EQ(-EEXIST, cls_llvm_exec(ioctx, oid1, bitcode, "create_c", inbl, reply_output, &rlog));
  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid1, bitcode, "create_cne", inbl, reply_output, &rlog));
}

TEST_F(ClsLLVM, Stat) 
{
  /* build object and stat */
  char buf[1024];
  vector<string> rlog;
  bufferlist bl, inbl, msg, outbl;

  bl.append(buf, sizeof(buf));
  uint64_t size;
  time_t mtime;
  ASSERT_EQ(-2, ioctx.stat(oid1, &size, &mtime));
  ASSERT_EQ(0, ioctx.write_full(oid1, bl));
  ASSERT_EQ(0, ioctx.stat(oid1, &size, &mtime));

  /* test stat success */
  
  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid1, bitcode, "stat_ret", inbl, outbl, &rlog));
  
  StatRet *sr;
  outbl.copy(0, outbl.length(), msg);
  sr = stat_ret__unpack(NULL, msg.length(), (uint8_t*) msg.c_str());
  
  ASSERT_EQ(size, (uint64_t) sr->size);

  stat_ret__free_unpacked(sr, NULL);

  /* test object dne */
  ASSERT_EQ(-ENOENT, cls_llvm_exec(ioctx, "dne", bitcode, "stat_dne", inbl, outbl, &rlog));
}

TEST_F(ClsLLVM, Read) {
  /* put data into object */
  string msg = "This is a test message";
  vector<string> rlog;
  bufferlist bl, outbl;
  bl.append(msg.c_str(), msg.size());
  ASSERT_EQ(0, ioctx.write_full(oid1, bl));
  
  ASSERT_EQ(msg.length(), (size_t) cls_llvm_exec(ioctx, oid1, bitcode, "Read", bl, outbl, &rlog));

  /* check return */
  string ret_val(outbl.c_str());
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
  ASSERT_EQ(0, ioctx.omap_set(oid1, orig_map));

  /* now compare to what we put it */
  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid1, bitcode, "map_get_val_foo", bl, outbl, &rlog));

  /* check return */
  string ret_val(outbl.c_str());
  ASSERT_EQ(ret_val, msg);

  /* error case */
  // ASSERT_EQ(-ENOENT, cls_llvm_exec(ioctx, oid1, bitcode, "map_get_val_dne", bl, outbl, &rlog));
}

TEST_F(ClsLLVM, MapSetVal) {
  /* build some input value */
  bufferlist orig_val, outbl;
  vector<string> rlog;
  string in_val = "this is the original value yay";
  orig_val.append(in_val.data(), in_val.length());

  /* stuff the data into a map value in c++ handler*/
  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid1, bitcode, "map_set_val_foo", orig_val, outbl, &rlog));
  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid1, bitcode, "map_set_val_bar", orig_val, outbl, &rlog));

  /* grab the key now and compare to orig */
  map<string, bufferlist> out_map;
  set<string> out_keys;
  out_keys.insert("foo");
  out_keys.insert("bar");
  ASSERT_EQ(0, ioctx.omap_get_vals_by_keys(oid1, out_keys, &out_map));
  bufferlist val_bl = out_map["bar"];
  string out_val;
  val_bl.copy(0, val_bl.length(), out_val);
  ASSERT_EQ(out_val, "this is the original value yay");
}

TEST_F(ClsLLVM, MapClear) {
  /* write some data into a key */
  string msg = "This is a test message";
  bufferlist val;
  val.append(msg.c_str(), msg.size());
  map<string, bufferlist> map;
  map["foo"] = val;
  ASSERT_EQ(0, ioctx.omap_set(oid1, map));

  /* test we can get it back out */
  set<string> keys;
  keys.insert("foo");
  map.clear();
  ASSERT_EQ(0, (int)map.count("foo"));
  ASSERT_EQ(0, ioctx.omap_get_vals_by_keys(oid1, keys, &map));
  ASSERT_EQ(1, (int)map.count("foo"));

  /* now clear it */
  bufferlist inbl, outbl;
  vector<string> rlog;

  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid1, bitcode, "map_clear", inbl, outbl, &rlog));
  
  /* test that the map we get back is empty now */
  map.clear();
  ASSERT_EQ(0, (int)map.count("foo"));
  ASSERT_EQ(0, ioctx.omap_get_vals_by_keys(oid1, keys, &map));
  ASSERT_EQ(0, (int)map.count("foo"));
}

TEST_F(ClsLLVM, TestLLVMPY) {
  bufferlist bitcodepy;
  bufferptr bp(cls_llvmpy_test_lib, cls_llvmpy_test_lib_size);
  bitcodepy.push_back(bp);

  /* put data into object */
  string msg = "This is a test message";
  vector<string> rlog;
  bufferlist bl1, bl2, outbl, keybl;

  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid1, bitcodepy, "write_0", bl1, outbl, &rlog));
  ASSERT_EQ(0, cls_llvm_exec(ioctx, oid2, bitcodepy, "write_1", bl1, outbl, &rlog));

  map<string, bufferlist> attrs;
  ASSERT_EQ(0, ioctx.getxattrs(oid1, attrs));
  
  int *d0 = (int*) attrs["d0"].c_str();
  int *d1 = (int*) attrs["d1"].c_str();
  
  ASSERT_EQ(4, *d0);
  ASSERT_EQ(7, *d1);
  
  ASSERT_EQ(28, cls_llvm_exec(ioctx, oid1, bitcodepy, "sum_0", bl1, outbl, &rlog));
  ASSERT_EQ(4096, cls_llvm_exec(ioctx, oid2, bitcodepy, "mul_1", bl1, outbl, &rlog));
}