#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Support/system_error.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/JIT.h>

#include "common/Mutex.h"
#include "objclass/objclass.h"
#include "cls_llvm_ops.h"

CLS_VER(1,0)
CLS_NAME(llvm)

cls_handle_t h_class;
cls_method_handle_t h_eval;

static int llvm_initialized;
static Mutex cls_llvm_lock("cls_llvm_lock");

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

  /* TODO: check return values here */
  llvm::llvm_start_multithreaded();
  llvm::InitializeNativeTarget();

  llvm_initialized = 1;
  return 0;
}

static int eval(cls_method_context_t hctx, bufferlist *in, bufferlist *out)
{
  cls_llvm_eval_op op;
  try {
    bufferlist::iterator it = in->begin();
    ::decode(op, it);
  } catch (buffer::error& err) {
    CLS_LOG(1, "ERROR: cls_llvm_eval_op(): failed to decode op");
    return -EINVAL;
  }

  int ret = llvm_initialize();
  if (ret) {
    CLS_LOG(1, "ERROR: llvm_initialize(): failed to init ret=%d", ret);
    return ret;
  }

  /*
   * Move the operation bitcode into LLVM data structures.
   * TODO: check return values. buf.get() != 0 ?
   */
  llvm::StringRef bc_str(op.bitcode.c_str(), op.bitcode.length());
  llvm::OwningPtr<llvm::MemoryBuffer> buf(llvm::MemoryBuffer::getMemBufferCopy(bc_str));

  /*
   * Parse the input bitcode.
   */
  string error;
  llvm::LLVMContext context;
  llvm::Module *module = llvm::ParseBitcodeFile(buf.get(), context, &error);
  if (!module) {
    CLS_LOG(1, "ERROR: could not parse input bitcode: %s", error.c_str());
    return -EINVAL;
  }

  llvm::ExecutionEngine *ee = llvm::ExecutionEngine::create(module);
  llvm::Function* function = ee->FindFunctionNamed(op.function.c_str());
  if (!function) {
    CLS_LOG(1, "ERROR: function named `%s` not found", op.function.c_str());
    return -EINVAL;
  }

  typedef int (*PFN)();
  PFN pfn = reinterpret_cast<PFN>(ee->getPointerToFunction(function));
  ret = pfn();

  CLS_LOG(0, "return value: %d", ret);

  delete ee;

  return 0;
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
