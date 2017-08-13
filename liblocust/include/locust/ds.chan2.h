#define CHAN2_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

#ifndef LOCKS_H
#include <locust/sched.locks.h>
#endif

typedef struct {
	const meta *type;
	int64 direction;
	deque rpending;
	deque spending;
} chan2;

#define chan2__type_id ((type_id const)3000)
META_DECL(chan2);

void chan2_init(chan2 *self, const meta *type);
void chan2_deinit(chan2 *self);


bool chan2_send(chan2 *self, const meta *type, ptr item);
bool chan2_recv(chan2 *self, const meta *type, ptr ret);

