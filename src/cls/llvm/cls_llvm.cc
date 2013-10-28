#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Support/system_error.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Type.h>
#include <llvm/Linker.h>

#include "common/Mutex.h"
#include "objclass/objclass.h"
#include "cls_llvm_ops.h"

CLS_VER(1,0)
CLS_NAME(llvm)

cls_handle_t h_class;
cls_method_handle_t h_eval;

static int llvm_initialized;
static Mutex cls_llvm_lock("cls_llvm_lock");

struct cls_llvm_eval_err {
   bool error;
   int ret;
};

struct cls_llvm_eval_ctx {
  cls_llvm_eval_op op;
  cls_llvm_eval_reply reply;
  cls_llvm_eval_err err;
  cls_llvm_eval_func pfn;
  cls_method_context_t hctx;
  cls_llvm_eval_ctx() {
    err.error = false;
    err.ret = 0;
  }

  cls_llvm_eval_ctx(cls_method_context_t hctx) {
    cls_llvm_eval_ctx();
    this->hctx = hctx;
  }
};

/*
 * Initialize LLVM
 *
 * This initialization occurs exactly once per OSD process. The intialization
 * is done the same way as in Cloudera Impala, but we should get a better feel
 * for what all of the restrictions are.
 */
static int llvm_initialize(void)
{
  Mutex::Locker lock(cls_llvm_lock);

  if (llvm_initialized)
    return 0;

  bool result = llvm::llvm_start_multithreaded();
  if (result) {
    CLS_LOG(0, "Multithreaded mode successfully enabled in LLVM, global lock set");
  } else {
    CLS_LOG(0, "LLVM threads disabled, multithreaded mode could not be enabled");
  }
    
  llvm::InitializeNativeTarget();

  llvm_initialized = 1;
  return 0;
}

static int eval(cls_method_context_t hctx, bufferlist *in, bufferlist *out)
{

  /*
   * the evaluation context
   */
  cls_llvm_eval_ctx ctx(hctx);

  /*
   * random memory we'll need later
   */
  int ret;
  string error;
  char msgbuf[64];

  /*
   * Instantiate some llvm data structures for the JIT
   */
  llvm::StringRef bc_str;
  llvm::MemoryBuffer *buf;
  llvm::LLVMContext context;
  llvm::Module *module;
  llvm::ExecutionEngine *ee;
  llvm::Function *function;

  /*
   * Decode the input bufferlist, logging errors both in Ceph
   * and in the reply.
   */
  try {
    bufferlist::iterator it = in->begin();
    ::decode(ctx.op, it);
  } catch (buffer::error& e) {
    CLS_LOG(0, "ERROR: cls_llvm_eval_op(): failed to decode op");
    ctx.reply.log.push_back("ERROR: cls_llvm_eval_op(): failed to decode op");
    ctx.err.error = true;
    ctx.err.ret = -EINVAL;
    goto out;
  }

  /*
   * Initialize llvm. Again loggin errors in both places.
   */
  ret = llvm_initialize();
  if (ret) {
    CLS_LOG(0, "ERROR: llvm_initialize(): failed to init ret=%d", ret);
    sprintf(msgbuf, "ERROR: llvm_initialize(): failed to init ret=%d", ret);
    ctx.reply.log.push_back(msgbuf);
    ctx.err.error = true;
    ctx.err.ret = ret;
    goto out;
  }

  /*
   * Move the operation bitcode into LLVM data structures.
   * TODO: check return values. buf.get() != 0 ?
   * ^^ not sure this is necessary. I don't think there's much going
   * on under the hood besides creating the MemoryBuffer.
   */
  bc_str = llvm::StringRef(ctx.op.bitcode.c_str(), ctx.op.bitcode.length());
  buf = llvm::MemoryBuffer::getMemBufferCopy(bc_str);

  /*
   * Parse the input bitcode.
   */
  module = llvm::ParseBitcodeFile(buf, context, &error);
  if (!module) {
    CLS_LOG(0, "ERROR: could not parse input bitcode: %s", error.c_str());
    sprintf(msgbuf, "ERROR: could not parse input bitcode: %s", error.c_str());
    ctx.reply.log.push_back(msgbuf);
    ctx.err.error = true;
    ctx.err.ret = -EINVAL;
    goto out;
  }

  /*
   * Create the JIT and find the named function in the input bitcode.
   */
  ee = llvm::ExecutionEngine::create(module);
  function = ee->FindFunctionNamed(ctx.op.function.c_str());
  if (!function) {
    CLS_LOG(0, "ERROR: function named `%s` not found", ctx.op.function.c_str());
    sprintf(msgbuf, "ERROR: function name `%s` not found", ctx.op.function.c_str());
    ctx.reply.log.push_back(msgbuf);
    ctx.err.error = true;
    ctx.err.ret = -EINVAL;
    goto out;
  }

  /*
   * Get a pointer to the named function and call it.
   * Log the return value both with the OSD and the reply.
   */
  ctx.pfn = reinterpret_cast<cls_llvm_eval_func>(ee->getPointerToFunction(function));
  CLS_LOG(0, "found the function!");

  ret = ctx.pfn(ctx.hctx, &(ctx.op.input), &(ctx.reply.output));
  CLS_LOG(0, "we did it!! %s", ctx.reply.output.c_str());  

  CLS_LOG(0, "return value: %d", ret);
  sprintf(msgbuf, "return value: %d", ret);
  ctx.reply.log.push_back(msgbuf);

  delete ee;

  ::encode(ctx.reply, *out);
  return ret;

out:
  ::encode(ctx.reply, *out);
  return ctx.err.ret;
  
}

void __cls_init()
{
  CLS_LOG(0, "loading cls_llvm");

  llvm_initialized = 0;

  cls_register("llvm", &h_class);

  cls_register_cxx_method(h_class, "eval",
			  CLS_METHOD_RD | CLS_METHOD_WR,
			  eval, &h_eval);
}
