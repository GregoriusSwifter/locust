#define TASK_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

#ifndef CONTEXT_H
#include <locust/sched.context.h>
#endif

#ifndef DEQUE_H
#include <locust/ds.deque.h>
#endif

/*
 * Tasks
 * Copyright (c) 2005 Russ Cox, MIT; see LIBTASK_COPYRIGHT
 * Altered by AndrewBC - 2012
 */
typedef struct {
	uint64 id;
	const byte* name;
	void (*start_fn)(ptr);
	ptr start_arg;
	ptr data;
	uint64 stack_len;
	ptr stack;
    context ctx;
    bool is_ready;
    bool is_exiting;
    bool is_daemon;
    bool is_yielding;
} task;

#define task__type_id ((type_id const)23)
META_DECL(task);

void task_init(task *t, const byte *name, void (*fn)(ptr), ptr arg, uint64 stacksize);
void task_deinit(task *t);

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

task *task_curr(void);
void task_schedule(task *t);
void task_schedule_next(task *t);
void task_daemonize(task *t);
void task_switch_to(task *t);
void overloaded task_yield(task *t, bool reschedule);
static inline void overloaded task_yield(task *t) {
	task_yield(t, 1);
}
static inline void task_sleep(task *t) {
	task_yield(t, 0);
}
void task_exit_all();
void task_exit(task *t);
void task_needstack(task *t, int n);


/*
 * basic procs and threads
 */

void		taskmain(int argc, char *argv[]);
unsigned int	taskdelay(unsigned int);

/*
 * Threaded I/O.
 */
/*
int		fdread(int, void*, int);
int		fdread1(int, void*, int);
int		fdwrite(int, void*, int);
void		fdwait(int, int);
int		fdnoblock(int);

void		fdtask(void*);
*/
/*
 * Network dialing - sets non-blocking automatically
 */
/*
enum {
	UDP = 0,
	TCP = 1,
};

int		netannounce(int, char*, int);
int		netaccept(int, char*, int*);
int		netdial(int, char*, int);
int		netlookup(char*, uint32_t*);	// blocks entire program!
int		netdial(int, char*, int);
*/
