#include "gtest/gtest.h"
#include "test/librados/test.h"

#include "include/types.h"
#include "cls/llvm/cls_llvm_client.h"

#include "test_lib.h"

TEST(ClsLLVM, Basic)
{
  librados::Rados rados;
  librados::IoCtx ioctx;
  string pool_name = get_temp_pool_name();

  /* create pool */
  ASSERT_EQ("", create_one_pool_pp(pool_name, rados));
  ASSERT_EQ(0, rados.ioctx_create(pool_name.c_str(), ioctx));

  bufferlist bitcode, input;

  bufferptr bp(cls_llvm_test_lib, cls_llvm_test_lib_size);
  bitcode.push_back(bp);

  ASSERT_EQ(0, cls_llvm_eval(ioctx, "blah", bitcode, "log", input));
}
