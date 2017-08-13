#ifndef TASK_H
#include <locust/sched.task.h>
#endif

TYPE_ID_DEF(task);

META_FIELDS_DEF(task) = {
    13, {
        META_FIELD_DEF(task, id, uint64),
        META_FIELD_DEF(task, name, ptr), /* sizeof(const byte*) */
		META_FIELD_DEF(task, start_fn, ptr),
		META_FIELD_DEF(task, start_arg, ptr),
		META_FIELD_DEF(task, data, ptr),
        META_FIELD_DEF(task, stack_len, uint64),
        META_FIELD_DEF(task, stack, ptr),
        META_FIELD_DEF(task, ctx, context),
        META_FIELD_DEF(task, ex, exception),
        META_FIELD_DEF(task, is_ready, bool),
        META_FIELD_DEF(task, is_exiting, bool),
        META_FIELD_DEF(task, is_daemon, bool),
        META_FIELD_DEF(task, is_yielding, bool),
    }
};

META_DEF(task, & META_FIELDS_OF(task), nil);

#ifndef THREAD_H
#include <locust/sched.threads.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef SYSCALLS_H
#include <syscalls/syscalls.h>
#endif

#ifdef __TARGET_OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

int	taskdebuglevel;
int	taskcount;
int	tasknswitch;
int	taskexitval;

/*
 * Tasks
 * Copyright (c) 2005 Russ Cox, MIT; see LIBTASK_COPYRIGHT
 * Altered by AndrewBC - 2012
 */

static inline void task_run(task *self) {
    assert(self != nil);
	self->start_fn(self->start_arg);
	task_exit(self);
}

/*
task* task_create(const byte *name, void (*fn)(ptr), ptr arg, uint64 stack_len) {
    assert(name != nil);
    assert(fn != nil);

    task *t = mem_get(& META_OF(task), 1);
    assert(t != nil);
    task_init(t, name, fn, arg, stack_len);

	printf("task_create 1 \"%s\"\n", name);
    fflush(stdout);
	task_schedule(t);
	printf("task_create 2\n");
    fflush(stdout);
    return t;
}
*/


static uint64 taskidgen;

void task_init(task *self, const byte *name, void (*fn)(ptr), ptr arg, uint64 stack_len) {
    bool did_err;
    assert(self != nil);
    assert(name != nil);
    assert(fn != nil);
#ifdef __TARGET_OS_WINDOWS
	self->stack = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, (uint32)(stack_len>>32), (uint32)(stack_len), NULL);
	assert(self->stack != NULL);
#else
    SYSCALL_mmap(self->stack, did_err, NULL, stack_len, 0x02 | 0x01, 0x0002 | 0x1000, -1, 0); /* W | R */ /* private | anonymous */
    if (did_err) {
        error("MMAP ERR %p", self->stack);
    }
#endif
	self->stack_len = stack_len;
	self->id = ++taskidgen;
	self->name = name;
	self->is_daemon = 0;
    self->is_yielding = 0;
    self->is_exiting = 0;
    self->is_ready = 0;
	self->start_fn = fn;
	self->start_arg = arg;

	/* do a reasonable initialization */
	mem_set(&self->ctx, sizeof(context), 0);

	/* initialize with current context */
	context* ctx = &self->ctx;

	// There used to be sun specific code here. lulz.
	debug("task_init 1, func: %p\n", fn);

    /* leave a few words open on both ends */
	context_init(&self->ctx, self->stack_len-64, self->stack+32, fn, arg);
	debug("task_init 2\n");
    return;
}

void task_deinit(task *self) {
    int ret;
    bool did_err;
    assert(self != nil);
#ifdef __TARGET_OS_WINDOWS
	assert(UnmapViewOfFile(self->stack) != 0);
#else
    SYSCALL_munmap(ret, did_err, self->stack, self->stack_len);
    assert(did_err == false);
    assert(ret == 0);
#endif
	return;
}

task* task_curr(void) {
	return (&thread_curr)->curr_task;
}

void task_schedule(task *t) {
    assert(t != nil);
	thread *curr_thread = &thread_curr;
	assert(curr_thread != nil);
    t->is_ready = true;
    deque_back_push(&curr_thread->tasks, & META_OF(ptr), &t);
}

void task_schedule_next(task *t) {
    assert(t != nil);
	thread *curr_thread = &thread_curr;
	assert(curr_thread != nil);
	t->is_ready = true;
    deque_front_push(&curr_thread->tasks, & META_OF(ptr), &t);
}

void task_daemonize(task *t) {
    assert(t != nil);
    t->is_daemon = true;
}

void task_switch_to(task *t) {
    assert(t != nil);

	thread *thr = &thread_curr;
	assert(thr != nil);
	task *tsk = thr->curr_task;
	debug("\ntask: %ld \"%s\" -> %ld \"%s\"\n", tsk->id, tsk->name, t->id, t->name);
	thr->curr_task = t;
	context_swap(&tsk->ctx, &t->ctx);
	thr->curr_task = tsk;
}

void overloaded task_yield(task *t, bool reschedule) {
    if (t == nil) {
        error("t == nil");
        return;
    }
	if (reschedule) {
		task_schedule(t);
	}
	t->is_yielding = true;
	task_needstack(t, 0);
    t->is_yielding = false;
    t->is_ready = false;
	thread *curr_thread = &thread_curr;
	assert(curr_thread != nil);
	context_swap(&t->ctx, curr_thread->sched_ctx);
}

void task_exit(task *t) {
    assert(t != nil);
    t->is_exiting = true;
	task_yield(t, false);
}

void task_exit_all(int return_value) {
    bool did_err;
    SYSCALL_exit(return_value, did_err, return_value);
}

void task_needstack(task *t, int n) {
    assert(t != nil);
	if((char*)&t <= (char*)t->stack
	|| (char*)&t - (char*)t->stack < 256+n){
	    die("task stack overflow: &t=%p tstk=%p n=%d\n", &t, t->stack, 256+n);
	}
}

/*
 * debugging
 */
/*
static void taskinfo(int s) {
	int i;
	task *t;
	char *extra;

	debug("task list:");
	for(i=0; i<nalltask; i++){
		t = alltask[i];
		if(t == taskrunning)
			extra = " (running)";
		else if(t->ready)
			extra = " (ready)";
		else
			extra = "";
		debug("%6d%c %-20s %s%s",
			t->id, t->system ? 's' : ' ',
			t->name, t->state, extra);
	}
}*/

