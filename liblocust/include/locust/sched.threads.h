#define THREADS_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

#ifndef TASK_H
#include <locust/sched.task.h>
#endif

#ifndef CONTEXT_H
#include <locust/sched.context.h>
#endif

#ifndef EXCEPTION_H
#include <locust/rt.exception.h>
#endif

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

/* thread */
typedef struct thread {
	uint64 id;
	task *curr_task;
	context *sched_ctx;
	struct thread* thief;
	struct thread* victim;
    exception ex;
	deque tasks;
    blockade_i *mem;
} thread;

#define thread__type_id ((type_id const)22)
META_DECL(thread);

extern __thread thread thread_curr;
extern thread *__threads;
extern uint64 __threads_count;

void threads_init(uint64 thread_count);
void threads_deinit(void);
void threads_launch(void);
thread* threads_curr(void);
//void threads_run(thread *context);
