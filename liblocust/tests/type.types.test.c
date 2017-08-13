#include <stddef.h>
#include <stdlib.h>
#include <locust/rt.test.h>

char *test_dlopen()
{

    return NULL;
}

char *test_functions()
{

    return NULL;
}

char *test_failures()
{

    return NULL;
}

char *test_dlclose()
{

    return NULL;
}

tests_start();
test_run(test_dlopen);
test_run(test_functions);
test_run(test_failures);
test_run(test_dlclose);
tests_end()
