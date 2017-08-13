#ifndef WAITS_H
#include <locust/sched.waits.h>
#endif

#ifndef INSPECT_H
#include <locust/rt.inspect.h>
#endif

#ifndef TASK_H
#include <locust/sched.task.h>
#endif

#ifndef SYSCALLS_H
#include <syscalls/syscalls.h>
#endif

extern uint64 threads_count;

struct timespec {
    int64 tv_sec; /* seconds */
    int64 tv_nsec;/* and nanoseconds */
};

void wait_gradual(uint8 rate) {
    // POSIX implementation: Need other portable implementations
    struct timespec req;
    bool did_err;
    int ret;

    if ( threads_count > 1 && rate < 8) {
        // Since we have multiple processors, we can do active spinning in
        // the hopes that someone else will do whatever we're deferring for
        wait_active();
    } else if (rate < 16) {
        // If this hits because threads_count == 1,
        // We're either on a single processor system, or assuming we are, so
        // passive waiting is required, or the other tasks won't be able
        // to accomplish whatever we're deferring for
        task_yield(task_curr(), true);
    } else {
        rate -= 8;
        req.tv_sec = 0;
        req.tv_nsec = 1 + (rate / (255 - 8)) * 999999998; // wait from 1 nanosecond to 1 second, 4.1841 ms at a time.
        SYSCALL_nanosleep(ret, did_err, &req, nil);
    }
}




