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

/*
 * Generalized version of cls_llvm_exec a-la cls_lua.
 * Not certain that the above eval functions are needed.
 * TODO: Might be nice to overload this so that clients don't 
 * have to pass that log vector and an output BL when not needed.
 */

int cls_llvm_exec(librados::IoCtx& ioctx, const string &oid,
                  const bufferlist& bitcode, const string& function,
                  const bufferlist &input, bufferlist &output, vector<string> *log)
{
   int ret;
   bufferlist inbl;
   bufferlist outbl;

   cls_llvm_eval_op call;
   cls_llvm_eval_reply reply;

   call.bitcode = bitcode;
   call.function = function;
   call.input = input;
   ::encode(call, inbl);

   ret = ioctx.exec(oid, "llvm", "eval", inbl, outbl);
   
   if (!outbl.is_zero()) {
     bufferlist::iterator iter = outbl.begin();
     ::decode(reply, iter);

     iter = reply.output.begin();
     iter.copy_all(output);
   }
   
   if (log)
      log->swap(reply.log);
   
   return ret;
}

        