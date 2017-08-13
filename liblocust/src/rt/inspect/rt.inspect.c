#ifndef INSPECT_H
#include <locust/rt.inspect.h>
#endif

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>

#ifdef __TARGET_OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#ifdef __TARGET_OS_FREEBSD
#include <sys/sysctl.h>
#endif

#ifdef __TARGET_OS_LINUX
#include <unistd.h>
#endif


static void get_proc_info_();
static void get_cache_info_();

uint32 proc_total=0;
uint32 proc_active=0;
uint32 cache_line_size=0;

#define DEFAULT_CACHE_LINE_SIZE 64

void static get_proc_info_() {
#ifdef __TARGET_OS_WINDOWS
    // Oh windows, you so funny.
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    proc_total = (uint32)sys_info.dwNumberOfProcessors;

    // Win32 provides a 31 bit bitmask of active processors. ... kay.
    unsigned int active_mask;
    unsigned int active_count;
    for (active_count = 0, active_mask = sys_info.dwActiveProcessorMask; active_mask; active_count++) {
        active_mask &= active_mask - 1; // Clear least significant bit set
    }
    proc_active = (uint32)active_count;
#endif
#ifdef __TARGET_OS_FREEBSD
    size_t len1 = sizeof(proc_active);
    size_t len2 = sizeof(proc_total);
    sysctlbyname("hw.activecpu", &proc_active, &len1, NULL, 0);
    sysctlbyname("hw.ncpu", &proc_total, &len2, NULL, 0);
#endif
#ifdef __TARGET_OS_LINUX
    // Posix compliant interface
    proc_active = sysconf(_SC_NPROCESSORS_ONLN);
    proc_total = sysconf(_SC_NPROCESSORS_CONF);
#endif

    if (proc_total < 1) {
        warn("Could not determine number of CPUs total: %s ... Using 1 as safe default. Got: %u\n", strerror(errno), proc_total);
        proc_total = 1;
    }

    if (proc_active < 1) {
        warn("Could not determine number of CPUs active: %s ... Using proc_total: %d Got %u\n", strerror(errno), proc_total, proc_active);
        proc_active = proc_total;
    }

    debug("%u of %u processors online\n", proc_active, proc_total);
}


void static get_cache_info_() {
#ifdef __TARGET_OS_WINDOWS
    size_t line_size = 0;
    DWORD buffer_size = 0;
    DWORD i = 0;
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION *buffer = 0;

    GetLogicalProcessorInformation(0, &buffer_size);
    buffer = mem_get(buffer_size);
    if (buffer == nil) { error("Error mallocing buffer for cache info.\n"); goto error; }
    GetLogicalProcessorInformation(buffer, &buffer_size);

    for (i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
        if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
            cache_line_size = buffer[i].Cache.LineSize;
            break;
        }
    }

    free(buffer);
#endif
#ifdef __TARGET_OS_FREEBSD
    size_t sizeof_line_size = sizeof(cache_line_size);
    sysctlbyname("hw.cachelinesize", &cache_line_size, &sizeof_line_size, 0, 0);
#endif
#ifdef __TARGET_OS_LINUX
    cache_line_size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
#endif

error:
    if (cache_line_size < 1) {
        // Something bad happened, we're going to assume a hopefully sane value.
        cache_line_size = DEFAULT_CACHE_LINE_SIZE;
        warn("Could not determine cache line size: %s ... assuming a value of %u", strerror(errno), cache_line_size);
    }

    debug("%u bytes cache line size\n", cache_line_size);
    return;
}


uint32 inspect_proc_total() {
    if (proc_total > 0) {
        return proc_total;
    }
    get_proc_info_();
    return proc_total;
}

uint32 inspect_proc_active() {
    if (proc_active > 0) {
        return proc_active;
    }
    get_proc_info_();
    return proc_active;
}

uint32 inspect_cache_line_size() {
    if (cache_line_size > 0) {
        return cache_line_size;
    }
    get_cache_info_();
    return cache_line_size;
}
