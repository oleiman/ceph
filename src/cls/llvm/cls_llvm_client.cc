#include "include/types.h"
#include "cls/llvm/cls_llvm_ops.h"
#include "include/rados/librados.hpp"

void cls_llvm_eval(librados::ObjectWriteOperation& op, const bufferlist& bitcode,
    const string& function, const bufferlist& input)
{
  bufferlist in;
  cls_llvm_eval_op call;
  call.bitcode = bitcode;
  call.function = function;
  call.input = input;
  ::encode(call, in);
  op.exec("llvm", "eval", in);
}

int cls_llvm_eval(librados::IoCtx& ioctx, const string& oid,
    const bufferlist& bitcode, const string& function, const bufferlist& input)
{
  librados::ObjectWriteOperation op;
  cls_llvm_eval(op, bitcode, function, input);
  return ioctx.operate(oid, &op);
}
