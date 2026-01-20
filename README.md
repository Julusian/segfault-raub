# Segfault handler for Node.js

[![NPM](https://badge.fury.io/js/%40julusian%2Fsegfault-raub.svg)](https://badge.fury.io/js/%40julusian%2Fsegfault-raub)

```
npm i -s @julusian/segfault-raub
```

This module installs platform-specific **signal** listeners
(see `sigaction` for **Unix** and `SetUnhandledExceptionFilter` for **Windows**).
Whenever a signal is raised, the module prints minimal crash information and a native stack trace to **STDERR**, then passes control to the default system handler for core dump generation.

**Key Design Goals:**

- **Zero heap allocations** in crash handler - uses only stack-allocated buffers and async-signal-safe system calls
- **No STL usage** in signal handler path - avoids `std::map`, `std::string`, `iostream`, etc.
- **Deterministic stack traces** - shows module name + offset (e.g., `module.node(+0x1234)`) for reproducible debugging across runs
- **Safe on corrupted heaps** - won't double-fault even when the V8 heap is corrupted

> Note: this **addon uses N-API**, and therefore is ABI-compatible across different
> Node.js versions. Addon binaries are precompiled and **there is no compilation**
> step during the `npm i` command.
>
> Maintained by [@Julusian](https://github.com/Julusian).

A zero-setup is available: just require the module and it comes pre-equipped with several
signal listeners enabled by default.

```javascript
require("@julusian/segfault-raub");
```

> Note: if your project tree contains multiple versions of this module, the first one imported
> will seize `global['@julusian/segfault-raub']`. The rest of them will only re-export `global['@julusian/segfault-raub']`
> and **WILL NOT** import their own **binaries**.

## Enabling Signals

As listed below, some signals (platform specific) are enabled by default. But they can be
enabled/disabled manually:

Example:

```javascript
const {
  setSignal,
  EXCEPTION_ACCESS_VIOLATION,
  SIGSEGV,
  EXCEPTION_BREAKPOINT,
  SIGTRAP,
} = require("@julusian/segfault-raub");

setSignal(EXCEPTION_ACCESS_VIOLATION, false);
setSignal(SIGSEGV, false);

setSignal(EXCEPTION_BREAKPOINT, true);
setSignal(SIGTRAP, true);
```

On **Windows**, all the **Unix** signals are `null`, and the opposite is true.
Passing `null` as the first parameter to `setSignal` **has no effect and is safe**.

## Demo Methods

These are be helpful to see how the signals are reported and if the log files are being written properly.

- `causeSegfault` - Causes a memory access violation.
- `causeDivisionInt` - Divides an integer by zero.
- `causeOverflow` - Runs infinite recursion (stack overflow).
- `causeIllegal` - Raises an "illegal instruction" exception.

Example:

```javascript
const { causeSegfault } = require("@julusian/segfault-raub");
causeSegfault();
```

## Windows Signals

| Signal                             | Enabled | Description                                   |
| :--------------------------------- | :-----: | :-------------------------------------------- |
| EXCEPTION_ACCESS_VIOLATION         |   yes   | Memory access was denied.                     |
| EXCEPTION_ARRAY_BOUNDS_EXCEEDED    |   yes   | Array was accessed with an illegal index.     |
| EXCEPTION_INT_DIVIDE_BY_ZERO       |   yes   | Attempt to divide by an integer divisor of 0. |
| EXCEPTION_ILLEGAL_INSTRUCTION      |   yes   | Attempt to execute an illegal instruction.    |
| EXCEPTION_NONCONTINUABLE_EXCEPTION |   yes   | Can't continue after an exception.            |
| EXCEPTION_STACK_OVERFLOW           |   yes   | The thread used up its stack.                 |
| EXCEPTION_INVALID_HANDLE           |   yes   | An invalid handle was specified.              |
| EXCEPTION_FLT_DIVIDE_BY_ZERO       |   no    | Attempt to divide by a float divisor of 0.f.  |
| EXCEPTION_DATATYPE_MISALIGNMENT    |   no    | A datatype misalignment was detected.         |
| EXCEPTION_BREAKPOINT               |   no    | A Breakpoint has been reached.                |
| EXCEPTION_SINGLE_STEP              |   no    | Continue single-stepping execution.           |
| EXCEPTION_FLT_DENORMAL_OPERAND     |   no    | One of the operands is denormal.              |
| EXCEPTION_FLT_INEXACT_RESULT       |   no    | The result cannot be represented exactly.     |
| EXCEPTION_FLT_INVALID_OPERATION    |   no    | Floating-point invalid operation.             |
| EXCEPTION_FLT_OVERFLOW             |   no    | The exponent of operation is too large.       |
| EXCEPTION_FLT_STACK_CHECK          |   no    | The Stack gone bad after a float operation.   |
| EXCEPTION_FLT_UNDERFLOW            |   no    | The exponent of operation is too low.         |
| EXCEPTION_INT_OVERFLOW             |   no    | The result of operation is too large.         |
| EXCEPTION_PRIV_INSTRUCTION         |   no    | Operation is not allowed in current mode.     |
| EXCEPTION_IN_PAGE_ERROR            |   no    | Can't access a memory page.                   |
| EXCEPTION_INVALID_DISPOSITION      |   no    | Invalid disposition returned.                 |
| EXCEPTION_GUARD_PAGE               |   no    | Accessing PAGE_GUARD-allocated modifier.      |

## Unix Signals

**Note:** SIGABRT is intentionally **disabled by default** to allow `abort()` to chain to Node.js's crash reporting mechanism after the handler prints crash info.

| Signal    | Handling | Enabled | Description                                                       |
| :-------- | :------: | :-----: | :---------------------------------------------------------------- |
| SIGABRT   |    A     |   no    | Process abort signal (disabled to allow Node.js crash reporting). |
| SIGFPE    |    A     |   yes   | Erroneous arithmetic operation.                                   |
| SIGSEGV   |    A     |   yes   | Invalid memory reference.                                         |
| SIGILL    |    A     |   yes   | Illegal instruction.                                              |
| SIGBUS    |    A     |   yes   | Access to an undefined portion of a memory object.                |
| SIGTERM   |    T     |   no    | Termination signal.                                               |
| SIGINT    |    T     |   no    | Terminal interrupt signal.                                        |
| SIGALRM   |    T     |   no    | Alarm clock.                                                      |
| SIGCHLD   |    I     |   no    | Child process terminated, stopped, or continued.                  |
| SIGCONT   |    C     |   no    | Continue executing, if stopped.                                   |
| SIGHUP    |    T     |   no    | Hangup.                                                           |
| SIGKILL   |    T     |   no    | Kill (cannot be caught or ignored).                               |
| SIGPIPE   |    T     |   no    | Write on a pipe with no one to read it.                           |
| SIGQUIT   |    A     |   no    | Terminal quit signal.                                             |
| SIGSTOP   |    S     |   no    | Stop executing (cannot be caught or ignored).                     |
| SIGTSTP   |    S     |   no    | Terminal stop signal.                                             |
| SIGTTIN   |    S     |   no    | Background process attempting read.                               |
| SIGTTOU   |    S     |   no    | Background process attempting write.                              |
| SIGUSR1   |    T     |   no    | User-defined signal 1.                                            |
| SIGUSR2   |    T     |   no    | User-defined signal 2.                                            |
| SIGPROF   |    T     |   no    | Profiling timer expired.                                          |
| SIGSYS    |    A     |   no    | Bad system call.                                                  |
| SIGTRAP   |    A     |   no    | Trace/breakpoint trap.                                            |
| SIGURG    |    I     |   no    | High bandwidth data is available at a socket.                     |
| SIGVTALRM |    T     |   no    | Virtual timer expired.                                            |
| SIGXCPU   |    A     |   no    | CPU time limit exceeded.                                          |
| SIGXFSZ   |    A     |   no    | File size limit exceeded.                                         |

- `T` - Abnormal termination of the process.
- `A` - Abnormal termination of the process with additional actions.
- `I` - Ignore the signal.
- `S` - Stop the process.
- `C` - Continue the process, if it is stopped; otherwise, ignore the signal.

## Legal notice

This package is derived from [segfault-handler](https://github.com/ddopson/node-segfault-handler).
The original licensing rules apply, see `LICENSE_node-segfault-handler`.

Also this project uses [callstack walker](https://github.com/JochenKalmbach/StackWalker)
which is licensed under BSD-2 Clause.

The rest of this package (the newly introduced files) is licensed under MIT.
