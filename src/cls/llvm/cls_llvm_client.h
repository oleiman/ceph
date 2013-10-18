#ifndef CEPH_CLS_LLVM_CLIENT_H
#define CEPH_CLS_LLVM_CLIENT_H

#include "include/types.h"
#include "cls/llvm/cls_llvm_ops.h"
#include "include/rados/librados.hpp"

void cls_llvm_eval(librados::ObjectWriteOperation& op, const bufferlist& bitcode,
    const string& function, const bufferlist& input);

int cls_llvm_eval(librados::IoCtx& ioctx, const string& oid,
    const bufferlist& bitcode, const string& function, const bufferlist& input);

int cls_llvm_exec(librados::IoCtx& ioctx, const string& oid,
		  const bufferlist& bitcode, const string& function, 
		  const bufferlist& input, bufferlist& output, vector<string> *log);

#endif
