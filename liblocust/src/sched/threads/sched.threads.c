#ifndef THREADS_H
#include <locust/sched.threads.h>
#endif

TYPE_ID_DEF(thread);

/*
typedef struct thread {
	uint64 id;
	task *curr_task;
	context *sched_ctx;
	struct thread* thief;
	struct thread* victim;
	deque tasks;
} thread;
*/
META_FIELDS_DEF(thread) = {
	5, {
		META_FIELD_DEF(thread, id, uint64),
		META_FIELD_DEF(thread, curr_task, ptr), // task*
		META_FIELD_DEF(thread, sched_ctx, ptr), // context*
		META_FIELD_DEF(thread, thief, ptr), // thread*
		META_FIELD_DEF(thread, victim, ptr), // thread*
		META_FIELD_DEF(thread, tasks, ptr), // deque*
    }
};

META_DEF(thread, & META_FIELDS_OF(thread), nil);

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef INSPECT_H
#include <locust/rt.inspect.h>
#endif

#ifndef WAITS_H
#include <locust/sched.waits.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#ifndef MAP_H
#include <locust/ds.map.h>
#endif

#ifndef SYSCALLS_H
#include <syscalls/syscalls.h>
#endif

bool threads_ready = false;
bool threads_go = false;

uint64 threads_count = 0;
uint64 threads_id_counter = 0;
__thread thread thread_curr;
thread* threads;

static int thread_scheduler(uint64 id);

void threads_init(uint64 thread_count) {
	if (thread_count == 0) { thread_count = 1; }
	threads_count = thread_count;

	threads = mem_get(sizeof(thread)*thread_count);
	if(threads == nil) { error("Error initializing memory for threads."); }

    mem_set(threads, sizeof(thread)*thread_count, 0);

	uint64 curr_id = 0;
	uint64 thief_id = thread_count-1;
	uint64 victim_id = 1;
	thread* t = threads;

	for (uint64 thread_counter=0; thread_counter < thread_count;) {
		// Make sure we know about adjacent threads, and that they know about us.
		// By the time every thread does this, everyone will know their neighbors.

		t->thief = threads+thief_id;
		t->thief->victim = t;

		t->victim = threads+victim_id;
		t->victim->thief = t;

		deque_init(&t->tasks, & META_OF(ptr));

        t->mem = blockade_init(256);

		++thread_counter;
		++t;
		++curr_id;
		thief_id = curr_id - 1;
		victim_id = (curr_id + 1) % threads_count;
	}

	threads_ready = true;
    return;
}

void threads_deinit(void) {
	mem_free(threads);
	threads = nil;
	threads_count = 0;
	return;
}

void threads_launch(void) {
    int ret;
    bool did_err;
    uint64 i;

	assert(threads_ready);
	debug("launching %ld threads", threads_count);

	for (i=1; i < threads_count; ++i) {
        SYSCALL_fork(ret, did_err);
        if (did_err || ret < 0) {
            error("Error spawning thread %lu. did_err: %d, ret: %d", i, did_err, ret);
            SYSCALL_exit(ret, did_err, -1);
        } else if (ret == 0) {
            thread_scheduler(i);
        }
    }
	debug("launching main thread");
	thread_scheduler(0);
}

static int thread_scheduler(uint64 id) {
	uint64 curr_id = id;
	uint64 thief_id = (curr_id == 0) ? threads_count : curr_id-1;
	uint64 victim_id = (curr_id + 1) % threads_count;
	thread* t = threads+curr_id;
	t->id = curr_id;
	thread_curr = *t;

	t->curr_task = nil;
	t->sched_ctx = nil; // This is set up later, inside the thread scheduler.

    // Inspired by code from http://www.1024cores.net/home/scalable-architecture/task-scheduling-strategies
	debug("inside thread %lu", curr_id);

	task *tsk;
	context ctx;
	context *old_ctx;
	context *tsk_ctx;
	t->sched_ctx = &ctx;
    uint64 backoff_rate = 0;

    // get ready, get set, go!
    if (curr_id == 0) {
    	threads_go = true;
    } else {
    	while (threads_go == false) {
    		wait_gradual(backoff_rate++);
    	}
    	backoff_rate = 0;
    }

	while (true) {
		debug("thread %lu getting task", curr_id);
		if (deque_front_pop(&t->tasks, & META_OF(ptr), &tsk) ||
			deque_front_pop(&t->victim->tasks, & META_OF(ptr), &tsk)) {
		    //repr(&tsk, & META_OF(task));
			debug("thread %lu got task: %ld named \"%s\" -- %ld and %ld tasks in deques", t->id, tsk->id, tsk->name, t->tasks.len, t->victim->tasks.len);
			tsk->is_ready = false;
			t->curr_task = tsk;
			debug("thread %lu context_swap", t->id);

			tsk_ctx = &tsk->ctx;
			old_ctx = &ctx;

			context_swap(old_ctx, tsk_ctx);
			debug("thread %lu context_swap done!", t->id);

			t->curr_task = nil;
            backoff_rate = 0;
        } else {
			debug("thread %lu waiting", t->id);
            wait_gradual(backoff_rate++);
        }
	}
	return 0;
}

thread* threads_curr(void) {
    return (&thread_curr);
}



















struct irq {
    int (*handler)(void *arg);
    void *args;
};

struct irq irqs[NR_IRQS];

void handle_irq(int irq)
{
    int irq_ret = 0;

    switch (irq) {
        case 0x20-0x40: // hardware IRQs
            if (irqs[irq - 0x20].handler)
                irq_ret = irqs[irq - 0x20].handler(irqs[irq - 0x20].args);

            // EOI the interrupt controller
            pic_EOI();
            break;

        case SYSCALL:
            // XXX do syscall stuff here
            break;

        // other vectors (SMP, gpf, page fault, etc)
    }

    // handle preemption
    if (irq_ret != 0) {
        // if any handler returns != 0, then either the hw interrupt handler driver wants to
        // force a reschedule because some hw event woke up a thread
        // or it was a scheduler timer, which would have fired inside the irq handler for
        // the hardware timer
        reschedule();
    }
}

timers:
// systemwide timers are simply a queue of events sorted by scheduled time to fire.
// the events are a function callback with an optional argument. The callbacks look
// exactly like an irq so the return code from a timer has the same effect as the
// return code from an irq handler.

struct timer {
    timer *next;
    int (*handler)(void *);
    void *arg;
    time_t scheduled_time;
};

struct timer *timer_queue;

// timer interrupt handler, registered on the PIT (or local apic timer) handler
int do_timer_irq(void *unused)
{
    int ret = 0;

    while (head of timer queue has expired) {
        timer = pop_head();

        // any != 0 return from a timer handler will carry up
        // to the interrupt glue.
        ret |= timer->handler(timer->args);
    }

    return ret;
}

thread sleep:

int thread_wakeup(void *arg)
{
    struct thread *t = (struct thread *)arg;

    insert into run queue(t);

    return 1; // force a reschedule
}

void thread_sleep(time_t delay) {
    struct timer;

    int int_state = disable_ints();

    queue_timer(&timer, delay, &thread_wakeup, (void *)current_thread);
    set thread state to sleep(current_thread);

    // since the thread wasn't put back in the run queue, a reschedule here will suspend
    // the current thread until the thread_wakeup timer event is called.
    reschedule();

    restore_ints(int_state);
}

