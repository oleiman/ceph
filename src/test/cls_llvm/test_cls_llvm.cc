#include "gtest/gtest.h"
#include "test/librados/test.h"

#include "include/types.h"
#include "cls/llvm/cls_llvm_client.h"

#include "test_lib.h"

TEST(ClsLLVM, ReturnInt)
{
  librados::Rados rados;
  librados::IoCtx ioctx;
  string pool_name = get_temp_pool_name();
  vector<string> args;
  vector<string> rlog;
  string ret_val;

  /* create pool */
  ASSERT_EQ("", create_one_pool_pp(pool_name, rados));
  ASSERT_EQ(0, rados.ioctx_create(pool_name.c_str(), ioctx));

  bufferlist bitcode, input, output;
  
  ::encode(args, input);

  bufferptr bp(cls_llvm_test_lib, cls_llvm_test_lib_size);
  bitcode.push_back(bp);

  ASSERT_EQ(0, cls_llvm_exec(ioctx, "blah", bitcode, "log", input, output, &rlog));

  bufferlist::iterator iter = output.begin();
  ::decode(ret_val, iter);
  
  ASSERT_EQ("-5", ret_val);
  
  ASSERT_EQ("return value: -5", rlog[0]);
  
}

TEST(ClsLLVM, ReturnStringArg)
{
  librados::Rados rados;
  librados::IoCtx ioctx;
  string pool_name = get_temp_pool_name();
  vector<string> args;
  vector<string> rlog;
  string ret_val;

  /* create pool */
  ASSERT_EQ("", create_one_pool_pp(pool_name, rados));
  ASSERT_EQ(0, rados.ioctx_create(pool_name.c_str(), ioctx));

  bufferlist bitcode, input, output;
  
  args.push_back("Judy");
  ::encode(args, input);

  bufferptr bp(cls_llvm_test_lib, cls_llvm_test_lib_size);
  bitcode.push_back(bp);

  ASSERT_EQ(0, cls_llvm_exec(ioctx, "blah", bitcode, "retStr", input, output, &rlog));

  bufferlist::iterator iter = output.begin();
  ::decode(ret_val, iter);
  
  ASSERT_EQ("Hello, Judy", ret_val);
  
  ASSERT_EQ("return value: -1", rlog[0]);
  
}
