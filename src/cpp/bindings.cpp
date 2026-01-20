#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#endif

#include "segfault-handler.hpp"

Napi::Object initModule(Napi::Env env, Napi::Object exports)
{
	segfault::init();

	exports.DefineProperty(Napi::PropertyDescriptor::Function(env, exports, "causeSegfault", segfault::causeSegfault));
	exports.DefineProperty(Napi::PropertyDescriptor::Function(env, exports, "causeDivisionInt", segfault::causeDivisionInt));
	exports.DefineProperty(Napi::PropertyDescriptor::Function(env, exports, "causeOverflow", segfault::causeOverflow));
	exports.DefineProperty(Napi::PropertyDescriptor::Function(env, exports, "causeIllegal", segfault::causeIllegal));
	exports.DefineProperty(Napi::PropertyDescriptor::Function(env, exports, "setSignal", segfault::setSignal));

#ifdef _WIN32
	exports.Set("EXCEPTION_ACCESS_VIOLATION", static_cast<double>(EXCEPTION_ACCESS_VIOLATION));
	exports.Set("EXCEPTION_DATATYPE_MISALIGNMENT", static_cast<double>(EXCEPTION_DATATYPE_MISALIGNMENT));
	exports.Set("EXCEPTION_BREAKPOINT", static_cast<double>(EXCEPTION_BREAKPOINT));
	exports.Set("EXCEPTION_SINGLE_STEP", static_cast<double>(EXCEPTION_SINGLE_STEP));
	exports.Set("EXCEPTION_ARRAY_BOUNDS_EXCEEDED", static_cast<double>(EXCEPTION_ARRAY_BOUNDS_EXCEEDED));
	exports.Set("EXCEPTION_FLT_DENORMAL_OPERAND", static_cast<double>(EXCEPTION_FLT_DENORMAL_OPERAND));
	exports.Set("EXCEPTION_FLT_DIVIDE_BY_ZERO", static_cast<double>(EXCEPTION_FLT_DIVIDE_BY_ZERO));
	exports.Set("EXCEPTION_FLT_INEXACT_RESULT", static_cast<double>(EXCEPTION_FLT_INEXACT_RESULT));
	exports.Set("EXCEPTION_FLT_INVALID_OPERATION", static_cast<double>(EXCEPTION_FLT_INVALID_OPERATION));
	exports.Set("EXCEPTION_FLT_OVERFLOW", static_cast<double>(EXCEPTION_FLT_OVERFLOW));
	exports.Set("EXCEPTION_FLT_STACK_CHECK", static_cast<double>(EXCEPTION_FLT_STACK_CHECK));
	exports.Set("EXCEPTION_FLT_UNDERFLOW", static_cast<double>(EXCEPTION_FLT_UNDERFLOW));
	exports.Set("EXCEPTION_INT_DIVIDE_BY_ZERO", static_cast<double>(EXCEPTION_INT_DIVIDE_BY_ZERO));
	exports.Set("EXCEPTION_INT_OVERFLOW", static_cast<double>(EXCEPTION_INT_OVERFLOW));
	exports.Set("EXCEPTION_PRIV_INSTRUCTION", static_cast<double>(EXCEPTION_PRIV_INSTRUCTION));
	exports.Set("EXCEPTION_IN_PAGE_ERROR", static_cast<double>(EXCEPTION_IN_PAGE_ERROR));
	exports.Set("EXCEPTION_ILLEGAL_INSTRUCTION", static_cast<double>(EXCEPTION_ILLEGAL_INSTRUCTION));
	exports.Set("EXCEPTION_NONCONTINUABLE_EXCEPTION", static_cast<double>(EXCEPTION_NONCONTINUABLE_EXCEPTION));
	exports.Set("EXCEPTION_STACK_OVERFLOW", static_cast<double>(EXCEPTION_STACK_OVERFLOW));
	exports.Set("EXCEPTION_INVALID_DISPOSITION", static_cast<double>(EXCEPTION_INVALID_DISPOSITION));
	exports.Set("EXCEPTION_GUARD_PAGE", static_cast<double>(EXCEPTION_GUARD_PAGE));
	exports.Set("EXCEPTION_INVALID_HANDLE", static_cast<double>(EXCEPTION_INVALID_HANDLE));

	exports.Set("SIGINT", env.Null());
	exports.Set("SIGILL", env.Null());
	exports.Set("SIGABRT", env.Null());
	exports.Set("SIGFPE", env.Null());
	exports.Set("SIGSEGV", env.Null());
	exports.Set("SIGTERM", env.Null());
	exports.Set("SIGHUP", env.Null());
	exports.Set("SIGQUIT", env.Null());
	exports.Set("SIGTRAP", env.Null());
	exports.Set("SIGBUS", env.Null());
	exports.Set("SIGKILL", env.Null());
	exports.Set("SIGUSR1", env.Null());
	exports.Set("SIGUSR2", env.Null());
	exports.Set("SIGPIPE", env.Null());
	exports.Set("SIGALRM", env.Null());
	exports.Set("SIGCHLD", env.Null());
	exports.Set("SIGCONT", env.Null());
	exports.Set("SIGSTOP", env.Null());
	exports.Set("SIGTSTP", env.Null());
	exports.Set("SIGTTIN", env.Null());
	exports.Set("SIGTTOU", env.Null());
	exports.Set("SIGURG", env.Null());
	exports.Set("SIGXCPU", env.Null());
	exports.Set("SIGXFSZ", env.Null());
	exports.Set("SIGVTALRM", env.Null());
	exports.Set("SIGPROF", env.Null());
	exports.Set("SIGWINCH", env.Null());
	exports.Set("SIGSYS", env.Null());
#else
	exports.Set("EXCEPTION_ACCESS_VIOLATION", env.Null());
	exports.Set("EXCEPTION_DATATYPE_MISALIGNMENT", env.Null());
	exports.Set("EXCEPTION_BREAKPOINT", env.Null());
	exports.Set("EXCEPTION_SINGLE_STEP", env.Null());
	exports.Set("EXCEPTION_ARRAY_BOUNDS_EXCEEDED", env.Null());
	exports.Set("EXCEPTION_FLT_DENORMAL_OPERAND", env.Null());
	exports.Set("EXCEPTION_FLT_DIVIDE_BY_ZERO", env.Null());
	exports.Set("EXCEPTION_FLT_INEXACT_RESULT", env.Null());
	exports.Set("EXCEPTION_FLT_INVALID_OPERATION", env.Null());
	exports.Set("EXCEPTION_FLT_OVERFLOW", env.Null());
	exports.Set("EXCEPTION_FLT_STACK_CHECK", env.Null());
	exports.Set("EXCEPTION_FLT_UNDERFLOW", env.Null());
	exports.Set("EXCEPTION_INT_DIVIDE_BY_ZERO", env.Null());
	exports.Set("EXCEPTION_INT_OVERFLOW", env.Null());
	exports.Set("EXCEPTION_PRIV_INSTRUCTION", env.Null());
	exports.Set("EXCEPTION_IN_PAGE_ERROR", env.Null());
	exports.Set("EXCEPTION_ILLEGAL_INSTRUCTION", env.Null());
	exports.Set("EXCEPTION_NONCONTINUABLE_EXCEPTION", env.Null());
	exports.Set("EXCEPTION_STACK_OVERFLOW", env.Null());
	exports.Set("EXCEPTION_INVALID_DISPOSITION", env.Null());
	exports.Set("EXCEPTION_GUARD_PAGE", env.Null());
	exports.Set("EXCEPTION_INVALID_HANDLE", env.Null());

	exports.Set("SIGINT", static_cast<double>(SIGINT));
	exports.Set("SIGILL", static_cast<double>(SIGILL));
	exports.Set("SIGABRT", static_cast<double>(SIGABRT));
	exports.Set("SIGFPE", static_cast<double>(SIGFPE));
	exports.Set("SIGSEGV", static_cast<double>(SIGSEGV));
	exports.Set("SIGTERM", static_cast<double>(SIGTERM));
	exports.Set("SIGHUP", static_cast<double>(SIGHUP));
	exports.Set("SIGQUIT", static_cast<double>(SIGQUIT));
	exports.Set("SIGTRAP", static_cast<double>(SIGTRAP));
	exports.Set("SIGBUS", static_cast<double>(SIGBUS));
	exports.Set("SIGKILL", static_cast<double>(SIGKILL));
	exports.Set("SIGUSR1", static_cast<double>(SIGUSR1));
	exports.Set("SIGUSR2", static_cast<double>(SIGUSR2));
	exports.Set("SIGPIPE", static_cast<double>(SIGPIPE));
	exports.Set("SIGALRM", static_cast<double>(SIGALRM));
	exports.Set("SIGCHLD", static_cast<double>(SIGCHLD));
	exports.Set("SIGCONT", static_cast<double>(SIGCONT));
	exports.Set("SIGSTOP", static_cast<double>(SIGSTOP));
	exports.Set("SIGTSTP", static_cast<double>(SIGTSTP));
	exports.Set("SIGTTIN", static_cast<double>(SIGTTIN));
	exports.Set("SIGTTOU", static_cast<double>(SIGTTOU));
	exports.Set("SIGURG", static_cast<double>(SIGURG));
	exports.Set("SIGXCPU", static_cast<double>(SIGXCPU));
	exports.Set("SIGXFSZ", static_cast<double>(SIGXFSZ));
	exports.Set("SIGVTALRM", static_cast<double>(SIGVTALRM));
	exports.Set("SIGPROF", static_cast<double>(SIGPROF));
	exports.Set("SIGWINCH", static_cast<double>(SIGWINCH));
	exports.Set("SIGSYS", static_cast<double>(SIGSYS));
#endif

	return exports;
}

NODE_API_MODULE(julusian_segfault_handler, initModule)
