## Rados object classes

libcls_hello_la_SOURCES = cls/hello/cls_hello.cc
libcls_hello_la_LIBADD = $(PTHREAD_LIBS) $(EXTRALIBS)
libcls_hello_la_LDFLAGS = ${AM_LDFLAGS} -version-info 1:0:0 -export-symbols-regex '.*__cls_.*'
radoslib_LTLIBRARIES += libcls_hello.la

libcls_rbd_la_SOURCES = cls/rbd/cls_rbd.cc
libcls_rbd_la_LIBADD = $(PTHREAD_LIBS) $(EXTRALIBS)
libcls_rbd_la_LDFLAGS = ${AM_LDFLAGS} -version-info 1:0:0 -export-symbols-regex '.*__cls_.*'
radoslib_LTLIBRARIES += libcls_rbd.la

libcls_lock_la_SOURCES = cls/lock/cls_lock.cc
libcls_lock_la_LIBADD = $(PTHREAD_LIBS) $(EXTRALIBS)
libcls_lock_la_LDFLAGS = ${AM_LDFLAGS} -version-info 1:0:0 -export-symbols-regex '.*__cls_.*'
radoslib_LTLIBRARIES += libcls_lock.la

libcls_refcount_la_SOURCES = \
	cls/refcount/cls_refcount.cc \
	cls/refcount/cls_refcount_ops.cc \
	common/ceph_json.cc 
libcls_refcount_la_LIBADD = libjson_spirit.la $(PTHREAD_LIBS) $(EXTRALIBS)
libcls_refcount_la_LDFLAGS = ${AM_LDFLAGS} -version-info 1:0:0 -export-symbols-regex '.*__cls_.*'
radoslib_LTLIBRARIES += libcls_refcount.la

libcls_version_la_SOURCES = cls/version/cls_version.cc
libcls_version_la_LIBADD = $(PTHREAD_LIBS) $(EXTRALIBS)
libcls_version_la_LDFLAGS = ${AM_LDFLAGS} -version-info 1:0:0 -export-symbols-regex '.*__cls_.*'
radoslib_LTLIBRARIES += libcls_version.la

libcls_log_la_SOURCES = cls/log/cls_log.cc
libcls_log_la_LIBADD = $(PTHREAD_LIBS) $(EXTRALIBS)
libcls_log_la_LDFLAGS = ${AM_LDFLAGS} -version-info 1:0:0 -export-symbols-regex '.*__cls_.*'
radoslib_LTLIBRARIES += libcls_log.la

libcls_statelog_la_SOURCES = cls/statelog/cls_statelog.cc
libcls_statelog_la_LIBADD = $(PTHREAD_LIBS) $(EXTRALIBS)
libcls_statelog_la_LDFLAGS = ${AM_LDFLAGS} -version-info 1:0:0 -export-symbols-regex '.*__cls_.*'
radoslib_LTLIBRARIES += libcls_statelog.la

libcls_replica_log_la_SOURCES = cls/replica_log/cls_replica_log.cc
libcls_replica_log_la_LIBADD = $(PTHREAD_LIBS) $(EXTRALIBS)
libcls_replica_log_la_LDFLAGS = ${AM_LDFLAGS} -version-info 1:0:0 -export-symbols-regex '.*__cls_.*'
radoslib_LTLIBRARIES += libcls_replica_log.la

libcls_rgw_la_SOURCES = \
	cls/rgw/cls_rgw.cc \
	cls/rgw/cls_rgw_ops.cc \
	cls/rgw/cls_rgw_types.cc \
	common/ceph_json.cc
libcls_rgw_la_LIBADD = libjson_spirit.la $(PTHREAD_LIBS) $(EXTRALIBS)
libcls_rgw_la_LDFLAGS = ${AM_LDFLAGS} -version-info 1:0:0 -export-symbols-regex '.*__cls_.*'
radoslib_LTLIBRARIES += libcls_rgw.la

LLVM_LIBS = -lLLVMAsmParser -lLLVMInstrumentation -lLLVMLinker -lLLVMArchive -lLLVMBitReader -lLLVMDebugInfo -lLLVMJIT -lLLVMipo -lLLVMVectorize -lLLVMBitWriter -lLLVMTableGen -lLLVMHexagonCodeGen -lLLVMHexagonAsmPrinter -lLLVMHexagonDesc -lLLVMHexagonInfo -lLLVMPTXCodeGen -lLLVMPTXDesc -lLLVMPTXInfo -lLLVMPTXAsmPrinter -lLLVMMBlazeAsmParser -lLLVMMBlazeCodeGen -lLLVMMBlazeDisassembler -lLLVMMBlazeDesc -lLLVMMBlazeInfo -lLLVMMBlazeAsmPrinter -lLLVMCppBackendCodeGen -lLLVMCppBackendInfo -lLLVMMSP430CodeGen -lLLVMMSP430Desc -lLLVMMSP430AsmPrinter -lLLVMMSP430Info -lLLVMXCoreCodeGen -lLLVMXCoreDesc -lLLVMXCoreInfo -lLLVMCellSPUCodeGen -lLLVMCellSPUDesc -lLLVMCellSPUInfo -lLLVMMipsAsmParser -lLLVMMipsCodeGen -lLLVMMipsDesc -lLLVMMipsAsmPrinter -lLLVMMipsDisassembler -lLLVMMipsInfo -lLLVMARMDisassembler -lLLVMARMAsmParser -lLLVMARMCodeGen -lLLVMARMDesc -lLLVMARMAsmPrinter -lLLVMARMInfo -lLLVMPowerPCCodeGen -lLLVMPowerPCDesc -lLLVMPowerPCInfo -lLLVMPowerPCAsmPrinter -lLLVMSparcCodeGen -lLLVMSparcDesc -lLLVMSparcInfo -lLLVMX86Disassembler -lLLVMX86CodeGen -lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMX86AsmParser -lLLVMX86Desc -lLLVMX86Info -lLLVMX86AsmPrinter -lLLVMX86Utils -lLLVMMCDisassembler -lLLVMMCParser -lLLVMInterpreter -lLLVMCodeGen -lLLVMScalarOpts -lLLVMInstCombine -lLLVMTransformUtils -lLLVMipa -lLLVMAnalysis -lLLVMMCJIT -lLLVMRuntimeDyld -lLLVMExecutionEngine -lLLVMTarget -lLLVMMC -lLLVMObject -lLLVMCore -lLLVMSupport
LLVM_LDFLAGS = -L/usr/lib/llvm-3.1/lib  -lpthread -lffi -ldl -lm
LLVM_CXXFLAGS = -I/usr/lib/llvm-3.1/include  -DNDEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -g -O2 -fomit-frame-pointer -fvisibility-inlines-hidden -fPIC -Woverloaded-virtual

libcls_llvm_la_SOURCES = cls/llvm/cls_llvm.cc
libcls_llvm_la_CXXFLAGS = $(LLVM_CXXFLAGS) $(AM_CXXFLAGS)
libcls_llvm_la_LDFLAGS = $(LLVM_LDFLAGS) $(LLVM_LIBS) ${AM_LDFLAGS} -version-info 1:0:0 -export-symbols-regex '.*__cls_.*'
radoslib_LTLIBRARIES += libcls_llvm.la

## Rados object client classes

libcls_lock_client_la_SOURCES = \
	cls/lock/cls_lock_client.cc \
	cls/lock/cls_lock_types.cc \
	cls/lock/cls_lock_ops.cc
noinst_LTLIBRARIES += libcls_lock_client.la
DENCODER_DEPS += libcls_lock_client.la

libcls_refcount_client_la_SOURCES = \
	cls/refcount/cls_refcount_client.cc \
	cls/refcount/cls_refcount_ops.cc
noinst_LTLIBRARIES += libcls_refcount_client.la
DENCODER_DEPS += libcls_refcount_client.la

libcls_version_client_a_SOURCES =  \
	cls/version/cls_version_client.cc \
	cls/version/cls_version_types.cc
noinst_LIBRARIES += libcls_version_client.a

libcls_log_client_a_SOURCES = cls/log/cls_log_client.cc
noinst_LIBRARIES += libcls_log_client.a

libcls_statelog_client_a_SOURCES = cls/statelog/cls_statelog_client.cc
noinst_LIBRARIES += libcls_statelog_client.a

libcls_replica_log_client_a_SOURCES = \
	cls/replica_log/cls_replica_log_types.cc \
	cls/replica_log/cls_replica_log_ops.cc \
	cls/replica_log/cls_replica_log_client.cc
noinst_LIBRARIES += libcls_replica_log_client.a
DENCODER_DEPS += libcls_replica_log_client.a

libcls_rgw_client_la_SOURCES = \
	cls/rgw/cls_rgw_client.cc \
	cls/rgw/cls_rgw_types.cc \
	cls/rgw/cls_rgw_ops.cc
noinst_LTLIBRARIES += libcls_rgw_client.la
DENCODER_DEPS += libcls_rgw_client.la

libcls_rbd_client_la_SOURCES = cls/rbd/cls_rbd_client.cc
noinst_LTLIBRARIES += libcls_rbd_client.la

libcls_llvm_client_a_SOURCES = cls/llvm/cls_llvm_client.cc
noinst_LIBRARIES += libcls_llvm_client.a

noinst_HEADERS += \
	cls/lock/cls_lock_types.h \
	cls/lock/cls_lock_ops.h \
	cls/lock/cls_lock_client.h \
	cls/rbd/cls_rbd.h \
	cls/rbd/cls_rbd_client.h \
	cls/refcount/cls_refcount_ops.h \
	cls/refcount/cls_refcount_client.h \
	cls/version/cls_version_types.h \
	cls/version/cls_version_ops.h \
	cls/version/cls_version_client.h \
	cls/log/cls_log_types.h \
	cls/log/cls_log_ops.h \
	cls/log/cls_log_client.h \
	cls/statelog/cls_statelog_types.h \
	cls/statelog/cls_statelog_ops.h \
	cls/statelog/cls_statelog_client.h \
	cls/replica_log/cls_replica_log_types.h \
	cls/replica_log/cls_replica_log_ops.h \
	cls/replica_log/cls_replica_log_client.h \
	cls/rgw/cls_rgw_client.h \
	cls/rgw/cls_rgw_ops.h \
	cls/rgw/cls_rgw_types.h \
	cls/llvm/cls_llvm_ops.h \
	cls/llvm/cls_llvm_client.h

