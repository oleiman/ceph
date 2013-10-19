#include "gtest/gtest.h"
#include "test/librados/test.h"

#include "include/types.h"
#include "cls/llvm/cls_llvm_client.h"

#include "test_lib.h"

TEST(ClsLLVM, ReturnStringArg)
{
  librados::Rados rados;
  librados::IoCtx ioctx;
  string pool_name = get_temp_pool_name();
  string result;
  vector<string> rlog;

  /* create pool */
  ASSERT_EQ("", create_one_pool_pp(pool_name, rados));
  ASSERT_EQ(0, rados.ioctx_create(pool_name.c_str(), ioctx));

  bufferlist bitcode, input, output;

  bufferptr bp(cls_llvm_test_lib, cls_llvm_test_lib_size);
  bitcode.push_back(bp);

  bufferptr argbp("some message", 12);
  input.push_back(argbp);

  int ret = cls_llvm_exec(ioctx, "blah", bitcode, "retStr", input, output, &rlog);

  bufferlist::iterator iter = output.begin();
  ::decode(result, iter);
  
  ASSERT_EQ(0, ret);
  
  ASSERT_EQ("some message", result);
  
  ASSERT_EQ("return value: 0", rlog[0]);
}
