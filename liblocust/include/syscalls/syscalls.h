#ifdef __TARGET_OS_FREEBSD
#ifdef __TARGET_ARCH_X86_64
#include "syscalls/syscalls.freebsd.x86_64.h"
#endif
#ifdef __TARGET_ARCH_X86
#include "syscalls/syscalls.freebsd.x86.h"
#endif
#include "syscalls/syscalls.freebsd.h"
#endif

#ifdef __TARGET_OS_LINUX
#ifdef __TARGET_ARCH_X86_64
#include "syscalls/syscalls.linux.x86_64.h"
#endif
#ifdef __TARGET_ARCH_X86
#include "syscalls/syscalls.linux.x86.h"
#endif
#include "syscalls/syscalls.linux.h"
#endif

#ifdef __TARGET_OS_WINDOWS
#ifdef __TARGET_ARCH_X86_64
#include "syscalls/syscalls.windows.x86_64.h"
#endif
#ifdef __TARGET_ARCH_X86
#include "syscalls/syscalls.windows.x86.h"
#endif
#include "syscalls/syscalls.windows.h"
#endif