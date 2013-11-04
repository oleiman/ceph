#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "rados/buffer.h"
#include "cls_llvm_test.pb-c.h"

using namespace std;
using namespace ceph;

extern "C" int cls_read(void *, int, int, char**, int*);
extern "C" int cls_write(void *, int, int, char*);
extern "C" int cls_create(void *, int);
extern "C" int cls_stat(void *, uint64_t*, time_t*);
extern "C" int cls_map_get_val(void*, const char*, char**, int*);
extern "C" int cls_map_set_val(void*, const char*, char*, int);
extern "C" int cls_map_clear(void*);
extern "C" int cls_log(int level, const char *format, ...)
  __attribute__((__format__(printf, 2, 3)));

/*
 * This file is compiled into LLVM IR.
 */
extern "C" {

  int retStr(void *hctx, bufferlist *in, bufferlist *out)
  {
    bufferptr obp(in->c_str(), in->length());
    out->push_back(obp);
    return 0;
  }

  int Write(void *hctx, bufferlist *in, bufferlist *out)
  {
    return cls_write(hctx, 0, in->length(), in->c_str());
  }

  int Read(void *hctx, bufferlist *in, bufferlist *out)
  {
    char *buf; 
    int datalen; 
    int ret = cls_read(hctx, 0, in->length(), &buf, &datalen);
    out->append(buf, datalen);
    return ret;
  }

  int create_c(void *hctx, bufferlist *in, bufferlist *out)
  {
    return cls_create(hctx, 1);
  }

  int create_cne(void *hctx, bufferlist *in, bufferlist *out)
  {
    return cls_create(hctx, 0);
  }

  int stat_ret(void *hctx, bufferlist *in, bufferlist *out)
  {
    uint64_t size;
    time_t time;
    int ret;
    ret = cls_stat(hctx, &size, &time);

    /*
     * Instantiate the protocol buffer to hold the structured
     * stat response
     */
    StatRet sr = STAT_RET__INIT;
    sr.size = size;
    sr.mtime = time;
    size_t len = stat_ret__get_packed_size(&sr);

    /*
     * Pack the protocol buffer into raw bytes
     * Append those bytes to the return bufferlist
     */
    void *buf = malloc(len);
    stat_ret__pack(&sr, (uint8_t*) buf);
    out->append((char*) buf, len);
    
    return ret;
  }
  
  int stat_dne(void *hctx, bufferlist *in, bufferlist *out) 
  {
    uint64_t size;
    time_t mtime;
    return cls_stat(hctx, &size, &mtime);
  }

  int map_get_val_foo(void *hctx, bufferlist *in, bufferlist *out)
  {
    char *buf; 
    int datalen;
    int ret = cls_map_get_val(hctx, "foo", &buf, &datalen);
    out->append(strdup(buf), datalen);
    return ret;
  }

  int map_get_val_dne(void *hctx, bufferlist *in, bufferlist *out)
  {
    char *buf;
    int datalen;
    return cls_map_get_val(hctx, "bar", &buf, &datalen);
  }

  int map_set_val_foo(void *hctx, bufferlist *in, bufferlist *out)
  {
    return cls_map_set_val(hctx, "foo", in->c_str(), in->length());
  }

  int map_clear(void *hctx, bufferlist *in, bufferlist *out)
  {
    return cls_map_clear(hctx);
  }

}
