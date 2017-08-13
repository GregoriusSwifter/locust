#define TEST_H
#undef NDEBUG

#include <stdio.h>
#include <stdlib.h>

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#define test_run(test) info("-----%s", " " #test); \
    result = NULL; \
    if (NULL == (result = (test)())) { info("PASS"); ++tests_passed; } \
    else { ("FAIL: %s", result); ++tests_failed; } \
    ++tests_ran;

#define tests_start() int main(int argc, char *argv[]) { \
    argc = 1; \
    char *result = NULL; \
    info("----- TESTING: %s", argv[0]); \

#define tests_end() info("PASSED: %d / %d FAILED: %d / %d", tests_passed, tests_ran, tests_failed, tests_ran); \
    exit(tests_failed > 0); \
}

unsigned int tests_failed = 0;
unsigned int tests_passed = 0;
unsigned int tests_ran = 0;

//#define test_assert(test, message) if (!(test)) { error(message); return message; }
//#define test_run(test) log_quiet("---- %s", " " #test); message = test(); tests_run++; if (message) return message;
