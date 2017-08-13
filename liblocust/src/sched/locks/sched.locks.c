#ifndef LOCKS_H
#include <locust/sched.locks.h>
#endif

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef WAITS_H
#include <locust/sched.waits.h>
#endif

TYPE_ID_DEF(qlock);

META_FIELDS_DEF(qlock) = {
    2, {
    	META_FIELD_DEF(qlock, owner, ptr),
    	META_FIELD_DEF(qlock, tasks_waiting, deque),
    }
};

META_DEF(qlock, & META_FIELDS_OF(qlock), nil);




TYPE_ID_DEF(rwlock);

META_FIELDS_DEF(rwlock) = {
    4, {
    	META_FIELD_DEF(rwlock, readers, uint64),
        META_FIELD_DEF(rwlock, writer, ptr),
        META_FIELD_DEF(rwlock, rwaiting, deque),
        META_FIELD_DEF(rwlock, wwaiting, deque),
    }
};

META_DEF(rwlock, & META_FIELDS_OF(rwlock), nil);



TYPE_ID_DEF(rendez);

META_FIELDS_DEF(rendez) = {
    3, {
    	META_FIELD_DEF(rendez, owner, ptr),
    	META_FIELD_DEF(rendez, tasks_waiting, deque),
    	META_FIELD_DEF(rendez, locking, bool),
    }
};

META_DEF(rendez, & META_FIELDS_OF(rendez), nil);


// qlock

void nonnullfunc qlock_init(qlock *self) {
    assert(self != nil);
	deque_init(&self->tasks_waiting, & META_OF(ptr));
}

void nonnullfunc qlock_deinit(qlock *self) {
    assert(self != nil);
	deque_deinit(&self->tasks_waiting);
}

static bool nonnullfunc _qlock(qlock *l, bool block) {
	assert(l != nil);
	task *t = task_curr();
	if(l->owner == nil){
		l->owner = t;
		return true;
	}
	if(!block) {
		return false;
	}
	deque_back_push(&l->tasks_waiting, & META_OF(ptr), t);
	task_sleep(t);
	assert(l->owner == task_curr());
	return true;
}

void nonnullfunc qlock_on(qlock *l) {
	assert(l != nil);
	_qlock(l, true);
}

bool nonnullfunc qlock_can(qlock *l) {
	assert(l != nil);
	return _qlock(l, false);
}

void nonnullfunc qlock_off(qlock *l) {
	assert(l != nil);
	assert(l->owner != nil);

	task *ready;
	if (deque_front_pop(&l->tasks_waiting, & META_OF(ptr), &ready)) {
		task_schedule(ready);
	}
}



// rwlock
rwlock* rwlock_create(void) {
	rwlock *l = mem_get(1, & META_OF(rwlock));
	assert(l != nil);
	mem_set(l, sizeof(rwlock), 0);
	rwlock_init(l);
	return l;
}

void nonnullfunc rwlock_init(rwlock *self) {
	assert(self != nil);
	deque_init(&self->rwaiting, & META_OF(ptr));
	deque_init(&self->wwaiting, & META_OF(ptr));
}

void nonnullfunc rwlock_deinit(rwlock *self) {
	assert(self != nil);
	deque_deinit(&self->rwaiting);
	deque_deinit(&self->wwaiting);
}

void nonnullfunc rwlock_destroy(rwlock *self) {
	assert(self != nil);
	rwlock_deinit(self);
	mem_free(self);
}


static bool nonnullfunc _rlock(rwlock *l, bool block) {
	assert(l != nil);
	task *t = task_curr();
	if(l->writer == nil && l->wwaiting.len == 0){
		l->readers++;
		return true;
	}
	if(!block) {
		return false;
	}
	deque_back_push(&l->rwaiting, & META_OF(ptr), t);
	task_sleep(t);
	return true;
}

void nonnullfunc rwlock_r_on(rwlock *l) {
	_rlock(l, true);
}

bool nonnullfunc rwlock_r_can(rwlock *l) {
	return _rlock(l, false);
}

void nonnullfunc rwlock_r_off(rwlock *l) {
	task *ready;
	if(--l->readers == 0 && deque_front_pop(&l->wwaiting, & META_OF(ptr), &ready)){
		l->writer = ready;
		task_schedule(ready);
	}
}


static int nonnullfunc _wlock(rwlock *l, bool block) {
	assert(l != nil);
	task *t = task_curr();

	if(l->writer == nil && l->readers == 0) {
		l->writer = t;
		return true;
	}

	if(!block) {
		return false;
	}

	deque_back_push(&l->wwaiting, & META_OF(ptr), t);
	task_sleep(t);
	return true;
}

void nonnullfunc rwlock_w_on(rwlock *l) {
	_wlock(l, true);
}

bool nonnullfunc rwlock_w_can(rwlock *l) {
	return _wlock(l, false);
}

void nonnullfunc rwlock_w_off(rwlock *l) {
	assert(l != nil);
	assert(l->writer != nil); // not locked...
	l->writer = nil;
	assert(l->readers == 0); // readers active...

	task *ready;
	while (deque_front_pop(&l->rwaiting, & META_OF(ptr), &ready)) {
		++l->readers;
		task_schedule(ready);
	}

	if (l->readers == 0 && deque_front_pop(&l->wwaiting, & META_OF(ptr), &ready)) {
		l->writer = ready;
		task_schedule(ready);
	}
}


// rendezvous
rendez* rendez_create(bool locking) {
	rendez *r = mem_get(1, & META_OF(rendez));
	assert(r != nil);
	mem_set(r, sizeof(rendez), 0);
	rendez_init(r, locking);
	return r;
}

void nonnullfunc rendez_init(rendez *self, bool locking) {
    assert(self != nil);
	self->locking = locking;
	deque_init(&self->tasks_waiting, & META_OF(ptr));
}

void nonnullfunc rendez_deinit(rendez *self) {
    assert(self != nil);
	deque_deinit(&self->tasks_waiting);
}

void nonnullfunc rendez_destroy(rendez *self) {
    assert(self != nil);
	rendez_deinit(self);
	mem_free(self);
}



/*
void task_sleep(rendez *r) {
	assert(r != nil);
	task *t = task_curr();
	assert(t != nil);
	deque_back_push(&r->tasks_waiting, & META_OF(ptr), &t);
	if (r->locking) {
		qlock_off((qlock*)r);
	}
	task_switch(t);
	if (r->locking) {
		qlock_on((qlock*)r);
	}
}

static uint64 _taskwakeup(rendez *r, uint64 count) {
	task *t;
	uint64 i;
	for (i=0; i < count; ++i) {
		if (!deque_front_pop(&r->tasks_waiting, & META_OF(ptr), &t)) {
			break;
		}
		task_schedule(t);
	}
	return i;
}

overloaded bool task_wake(rendez *r) {
	return _taskwakeup(r, 1);
}

overloaded uint64 task_wake(rendez *r, uint64 count) {
	return _taskwakeup(r, count);
}
*/
