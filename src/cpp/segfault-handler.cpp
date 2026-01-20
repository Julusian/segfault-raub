#include <map>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#endif

#include "segfault-handler.hpp"

namespace segfault
{

#ifdef _WIN32
	constexpr auto GETPID = _getpid;
#define SEGFAULT_HANDLER LONG CALLBACK handleSignal(PEXCEPTION_POINTERS info)
#define NO_INLINE __declspec(noinline)
#define HANDLER_CANCEL return EXCEPTION_CONTINUE_SEARCH
#define HANDLER_DONE return EXCEPTION_EXECUTE_HANDLER
#else
	constexpr auto GETPID = getpid;
#define SEGFAULT_HANDLER static void handleSignal(int sig, siginfo_t *info, void *unused)
#define NO_INLINE __attribute__((noinline))
#define HANDLER_CANCEL return
#define HANDLER_DONE return

	int stackBytes = (int)SIGSTKSZ;
	char *_altStackBytes = new char[stackBytes];

	stack_t _altStack = {
		_altStackBytes,
		0,
		stackBytes,
	};
#endif

	const std::map<uint32_t, std::string> signalNames = {
#ifdef _WIN32
#define EXCEPTION_ALL 0x0
		{EXCEPTION_ALL, "CAPTURE ALL THE EXCEPTIONS"},
		{EXCEPTION_ACCESS_VIOLATION, "ACCESS_VIOLATION"},
		{EXCEPTION_DATATYPE_MISALIGNMENT, "DATATYPE_MISALIGNMENT"},
		{EXCEPTION_BREAKPOINT, "BREAKPOINT"},
		{EXCEPTION_SINGLE_STEP, "SINGLE_STEP"},
		{EXCEPTION_ARRAY_BOUNDS_EXCEEDED, "ARRAY_BOUNDS_EXCEEDED"},
		{EXCEPTION_FLT_DENORMAL_OPERAND, "FLT_DENORMAL_OPERAND"},
		{EXCEPTION_FLT_DIVIDE_BY_ZERO, "FLT_DIVIDE_BY_ZERO"},
		{EXCEPTION_FLT_INEXACT_RESULT, "FLT_INEXACT_RESULT"},
		{EXCEPTION_FLT_INVALID_OPERATION, "FLT_INVALID_OPERATION"},
		{EXCEPTION_FLT_OVERFLOW, "FLT_OVERFLOW"},
		{EXCEPTION_FLT_STACK_CHECK, "FLT_STACK_CHECK"},
		{EXCEPTION_FLT_UNDERFLOW, "FLT_UNDERFLOW"},
		{EXCEPTION_INT_DIVIDE_BY_ZERO, "INT_DIVIDE_BY_ZERO"},
		{EXCEPTION_INT_OVERFLOW, "INT_OVERFLOW"},
		{EXCEPTION_PRIV_INSTRUCTION, "PRIV_INSTRUCTION"},
		{EXCEPTION_IN_PAGE_ERROR, "IN_PAGE_ERROR"},
		{EXCEPTION_ILLEGAL_INSTRUCTION, "ILLEGAL_INSTRUCTION"},
		{EXCEPTION_NONCONTINUABLE_EXCEPTION, "NONCONTINUABLE_EXCEPTION"},
		{EXCEPTION_STACK_OVERFLOW, "STACK_OVERFLOW"},
		{EXCEPTION_INVALID_DISPOSITION, "INVALID_DISPOSITION"},
		{EXCEPTION_GUARD_PAGE, "GUARD_PAGE"},
		{EXCEPTION_INVALID_HANDLE, "INVALID_HANDLE"},
		{STATUS_STACK_BUFFER_OVERRUN, "STACK_BUFFER_OVERRUN"},
#else
		{SIGABRT, "SIGABRT"},
		{SIGFPE, "SIGFPE"},
		{SIGSEGV, "SIGSEGV"},
		{SIGTERM, "SIGTERM"},
		{SIGILL, "SIGILL"},
		{SIGINT, "SIGINT"},
		{SIGALRM, "SIGALRM"},
		{SIGBUS, "SIGBUS"},
		{SIGCHLD, "SIGCHLD"},
		{SIGCONT, "SIGCONT"},
		{SIGHUP, "SIGHUP"},
		{SIGKILL, "SIGKIL"},
		{SIGPIPE, "SIGPIPE"},
		{SIGQUIT, "SIGQUIT"},
		{SIGSTOP, "SIGSTOP"},
		{SIGTSTP, "SIGTSTP"},
		{SIGTTIN, "SIGTTIN"},
		{SIGTTOU, "SIGTTOU"},
		{SIGUSR1, "SIGUSR1"},
		{SIGUSR2, "SIGUSR2"},
		{SIGPROF, "SIGPROF"},
		{SIGSYS, "SIGSYS"},
		{SIGTRAP, "SIGTRAP"},
		{SIGURG, "SIGURG"},
		{SIGVTALRM, "SIGVTALRM"},
		{SIGXCPU, "SIGXCPU"},
		{SIGXFSZ, "SIGXFSZ"},
#endif
	};

	std::map<uint32_t, bool> signalActivity = {
#ifdef _WIN32
		{EXCEPTION_ALL, false},
		{EXCEPTION_ACCESS_VIOLATION, true},
		{EXCEPTION_ARRAY_BOUNDS_EXCEEDED, true},
		{EXCEPTION_FLT_DIVIDE_BY_ZERO, true},
		{EXCEPTION_INT_DIVIDE_BY_ZERO, true},
		{EXCEPTION_ILLEGAL_INSTRUCTION, true},
		{EXCEPTION_NONCONTINUABLE_EXCEPTION, true},
		{EXCEPTION_STACK_OVERFLOW, true},
		{EXCEPTION_INVALID_HANDLE, true},
		{EXCEPTION_DATATYPE_MISALIGNMENT, false},
		{EXCEPTION_BREAKPOINT, false},
		{EXCEPTION_SINGLE_STEP, false},
		{EXCEPTION_FLT_DENORMAL_OPERAND, false},
		{EXCEPTION_FLT_INEXACT_RESULT, false},
		{EXCEPTION_FLT_INVALID_OPERATION, false},
		{EXCEPTION_FLT_OVERFLOW, false},
		{EXCEPTION_FLT_STACK_CHECK, false},
		{EXCEPTION_FLT_UNDERFLOW, false},
		{EXCEPTION_INT_OVERFLOW, false},
		{EXCEPTION_PRIV_INSTRUCTION, false},
		{EXCEPTION_IN_PAGE_ERROR, false},
		{EXCEPTION_INVALID_DISPOSITION, false},
		{EXCEPTION_GUARD_PAGE, false},
		{STATUS_STACK_BUFFER_OVERRUN, false},
#else
		{SIGABRT, false}, // Don't catch SIGABRT - let abort() reach Node.js crash reporter
		{SIGFPE, true},
		{SIGSEGV, true},
		{SIGILL, true},
		{SIGBUS, true},
		{SIGTERM, false},
		{SIGINT, false},
		{SIGALRM, false},
		{SIGCHLD, false},
		{SIGCONT, false},
		{SIGHUP, false},
		{SIGKILL, false},
		{SIGPIPE, false},
		{SIGQUIT, false},
		{SIGSTOP, false},
		{SIGTSTP, false},
		{SIGTTIN, false},
		{SIGTTOU, false},
		{SIGUSR1, false},
		{SIGUSR2, false},
		{SIGPROF, false},
		{SIGSYS, false},
		{SIGTRAP, false},
		{SIGURG, false},
		{SIGVTALRM, false},
		{SIGXCPU, false},
		{SIGXFSZ, false},
#endif
	};

	// Pre-allocated buffer for safe crash output (no heap allocation)
	static char _crashBuf[512];
	static void *_backtrace[32]; // Pre-allocated buffer for backtrace addresses

	SEGFAULT_HANDLER
	{
		// SAFE crash handler: no allocations, no iostream, no STL, no map lookups
		// Extract signal info directly without using std::pair
		uint32_t signalId;
		uint64_t address;

#ifdef _WIN32
		signalId = static_cast<uint32_t>(info->ExceptionRecord->ExceptionCode);
		address = reinterpret_cast<uint64_t>(info->ExceptionRecord->ExceptionAddress);
#else
		signalId = static_cast<uint32_t>(info->si_signo);
		address = reinterpret_cast<uint64_t>(info->si_addr);
#endif

		// Just write minimal info and let Node.js handle the rest via abort()
		int pid = GETPID();
		int len = snprintf(_crashBuf, sizeof(_crashBuf),
						   "\n*** CRASH: PID %d, Signal %u, Address 0x%llx ***\n",
						   pid, signalId, (unsigned long long)address);

#ifdef _WIN32
		DWORD written;
		WriteFile(GetStdHandle(STD_ERROR_HANDLE), _crashBuf, len, &written, NULL);

		// Capture stack frames (no symbol resolution, just addresses)
		USHORT frames = CaptureStackBackTrace(1, 32, _backtrace, NULL); // Skip frame 0 (this handler)

		// Write stack frames with module info
		for (USHORT i = 0; i < frames; i++)
		{
			HMODULE hModule = NULL;
			uintptr_t addr = (uintptr_t)_backtrace[i];

			// GetModuleHandleEx is relatively safe - just looks up in already-loaded module list
			if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
								   (LPCSTR)addr, &hModule) &&
				hModule != NULL)
			{
				char moduleName[256];
				DWORD nameLen = GetModuleFileNameA(hModule, moduleName, sizeof(moduleName));
				if (nameLen > 0)
				{
					// Calculate offset from module base
					uintptr_t offset = addr - (uintptr_t)hModule;

					// Extract just the filename from full path
					char *fileName = moduleName;
					for (char *p = moduleName; *p; p++)
					{
						if (*p == '\\' || *p == '/')
							fileName = p + 1;
					}

					len = snprintf(_crashBuf, sizeof(_crashBuf), "%s(+0x%llx) [0x%llx]\n",
								   fileName, (unsigned long long)offset, (unsigned long long)addr);
				}
				else
				{
					len = snprintf(_crashBuf, sizeof(_crashBuf), "[0x%llx]\n", (unsigned long long)addr);
				}
			}
			else
			{
				len = snprintf(_crashBuf, sizeof(_crashBuf), "[0x%llx]\n", (unsigned long long)addr);
			}
			if (len > 0 && len < (int)sizeof(_crashBuf))
			{
				WriteFile(GetStdHandle(STD_ERROR_HANDLE), _crashBuf, len, &written, NULL);
			}
		}

		// Final newline
		WriteFile(GetStdHandle(STD_ERROR_HANDLE), "\n", 1, &written, NULL);

		// Don't call abort() - just return and let default handler take over
		HANDLER_CANCEL;
#else
		write(STDERR_FILENO, _crashBuf, len);

		// Get backtrace (async-signal-safe on most systems)
		int trace_size = backtrace(_backtrace, 32);

		// Write backtrace directly to stderr using backtrace_symbols_fd (async-signal-safe)
		if (trace_size > 0)
		{
			backtrace_symbols_fd(_backtrace, trace_size, STDERR_FILENO);
			write(STDERR_FILENO, "\n", 1); // Final newline
		}

		// Don't call abort() - just return and let default handler generate core dump
		HANDLER_CANCEL;
#endif
	}

	// create some stack frames to inspect from CauseSegfault
	NO_INLINE void _segfaultStackFrame1()
	{
		int *foo = reinterpret_cast<int *>(1);
		*foo = 42; // triggers a segfault exception
	}

	NO_INLINE void _segfaultStackFrame2(void)
	{
		void (*fn_ptr)() = _segfaultStackFrame1;
		fn_ptr();
	}

	JS_METHOD(causeSegfault)
	{
		Napi::Env env = info.Env();
		std::cout << "SegfaultHandler: about to cause a segfault..." << std::endl;
		void (*fn_ptr)() = _segfaultStackFrame2;
		fn_ptr();
		RET_UNDEFINED;
	}

	NO_INLINE void _divideInt()
	{
		volatile int a = 42;
		volatile int b = 0;
		a /= b;
	}

	JS_METHOD(causeDivisionInt)
	{
		Napi::Env env = info.Env();
		_divideInt();
		RET_UNDEFINED;
	}

	NO_INLINE void _overflowStack()
	{
		int foo[1000];
		foo[999] = 1;
		std::vector<int> empty = {foo[999]};
		if (empty.size() != 1)
		{
			return;
		}
		_overflowStack(); // infinite recursion
	}

	JS_METHOD(causeOverflow)
	{
		Napi::Env env = info.Env();
		std::cout << "SegfaultHandler: about to overflow the stack..." << std::endl;
		_overflowStack();
		RET_UNDEFINED;
	}

	JS_METHOD(causeIllegal)
	{
		Napi::Env env = info.Env();
		std::cout << "SegfaultHandler: about to raise an illegal operation..." << std::endl;
#ifdef _WIN32
		RaiseException(EXCEPTION_ILLEGAL_INSTRUCTION, 0, 0, nullptr);
#else
		raise(SIGILL);
#endif
		RET_UNDEFINED;
	}

	static inline void _enableSignal(uint32_t signalId)
	{
#ifndef _WIN32
		struct sigaction action;
		memset(&action, 0, sizeof(struct sigaction));
		sigemptyset(&action.sa_mask);
		action.sa_sigaction = handleSignal;

		if (signalId == SIGSEGV)
		{
			action.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_RESETHAND;
		}
		else
		{
			action.sa_flags = SA_SIGINFO | SA_RESETHAND;
		}

		sigaction(signalId, &action, NULL);
#endif
	}

	static inline void _disableSignal(uint32_t signalId)
	{
#ifndef _WIN32
		signal(signalId, SIG_DFL);
#endif
	}

	JS_METHOD(setSignal)
	{
		Napi::Env env = info.Env();
		if (IS_ARG_EMPTY(0))
		{
			RET_UNDEFINED;
		}

		LET_INT32_ARG(0, signalId);
		LET_BOOL_ARG(1, value);

		if (!signalNames.count(signalId))
		{
			RET_UNDEFINED;
		}

		bool wasEnabled = signalActivity[signalId];
		if (wasEnabled == value)
		{
			RET_UNDEFINED;
		}

		signalActivity[signalId] = value;
		if (value)
		{
			_enableSignal(signalId);
		}
		else
		{
			_disableSignal(signalId);
		}

		RET_UNDEFINED;
	}

	void init()
	{
		// On Windows, a single handler is set on startup.
		// On Unix, handlers for every signal are set on-demand.
#ifdef _WIN32
		SetUnhandledExceptionFilter(handleSignal);
#endif

		// `SetThreadStackGuarantee` and `sigaltstack` help in handling stack overflows on their platforms.
#ifdef _WIN32
		ULONG size = 32 * 1024;
		SetThreadStackGuarantee(&size);
#else
		sigaltstack(&_altStack, nullptr);
#endif

		for (auto pair : signalActivity)
		{
			if (pair.second)
			{
				_enableSignal(pair.first);
			}
		}
	}

} // namespace segfault
