#ifndef CEPH_CLS_LLVMJIT_OPS_H
#define CEPH_CLS_LLVMJIT_OPS_H

#include "include/types.h"

struct cls_llvm_eval_op {
  bufferlist bitcode;
  string function;
  bufferlist input;

  void encode(bufferlist& bl) const {
    ENCODE_START(1, 1, bl);
    ::encode(bitcode, bl);
    ::encode(function, bl);
    ::encode(input, bl);
    ENCODE_FINISH(bl);
  }

  void decode(bufferlist::iterator& bl) {
    DECODE_START(1, bl);
    ::decode(bitcode, bl);
    ::decode(function, bl);
    ::decode(input, bl);
    DECODE_FINISH(bl);
  }
};
WRITE_CLASS_ENCODER(cls_llvm_eval_op)

#endif
