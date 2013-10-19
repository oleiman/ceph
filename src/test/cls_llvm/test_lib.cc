#include <stdio.h>
#include <string.h>
#include <string>

#include "rados/buffer.h"

using namespace std;
using namespace ceph;

/*
 * This file is compiled into LLVM IR.
 */
extern "C" {

  int retStr(bufferlist *in, bufferlist *out)
  {
    bufferptr obp(in->c_str(), 12);
    out->push_back(obp);
    return 0;
  }

}
