#define LOCKS_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

#ifndef WAITS_H
#include <locust/sched.waits.h>
#endif

#ifndef ATOMIC_H
#include <locust/sched.atomic.h>
#endif

#ifndef TASK_H
#include <locust/sched.task.h>
#endif

#ifndef DEQUE_H
#include <locust/ds.deque.h>
#endif



/*
 * queuing locks
 */
typedef struct {
	task *owner;
	deque tasks_waiting; // task* deque
} qlock;

#define qlock__type_id ((type_id const)3001)
META_DECL(qlock);

void nonnullfunc qlock_init(qlock *self);
void nonnullfunc qlock_deinit(qlock *self);

void nonnullfunc qlock_on(qlock *self);
bool nonnullfunc qlock_can(qlock *self);
void nonnullfunc qlock_off(qlock *self);


/*
 * reader-writer locks
 */
typedef struct {
	uint64 readers;
	task* writer;
	deque rwaiting; // task* deque
	deque wwaiting; // task* deque
} rwlock;

#define rwlock__type_id ((type_id const)3002)
META_DECL(rwlock);

rwlock* rwlock_create(void);
void nonnullfunc rwlock_init(rwlock *self);
void nonnullfunc rwlock_deinit(rwlock *self);
void nonnullfunc rwlock_destroy(rwlock *self);

void nonnullfunc rwlock_r_on(rwlock*);
bool nonnullfunc rwlock_r_can(rwlock*);
void nonnullfunc rwlock_r_off(rwlock*);

void nonnullfunc rwlock_w_on(rwlock*);
bool nonnullfunc rwlock_w_can(rwlock*);
void nonnullfunc rwlock_w_off(rwlock*);

// simple lock
static inline bool nonnullfunc lock_try(bool* lock) {
    return atomic_swap((uint8*)lock, 1);
}

static inline void nonnullfunc lock_on(bool* lock) {
    uint8 wait_rate = 0;
    while(lock_try(lock)) {
        while(*lock) { wait_gradual(wait_rate++); }
    }
}

static inline void nonnullfunc lock_off(bool* lock) {
    atomic_r_barrier();
    *lock = 0;
}


// rendezvous (looks like a qlock, too)
typedef struct {
	task *owner;
	deque tasks_waiting; // task* deque
	bool locking;
} rendez;

#define rendez__type_id ((type_id const)3003)
META_DECL(rendez);

rendez* rendez_create(bool locking);
void nonnullfunc rendez_init(rendez *r, bool locking);
void nonnullfunc rendez_deinit(rendez *r);
void nonnullfunc rendez_destroy(rendez *r);

/*
void task_sleep(rendez*);
overloaded bool nonnullfunc task_wake(rendez *r);
overloaded uint64 nonnullfunc task_wake(rendez *r, uint64 count);
*/
