#ifndef CONTEXT_H
#include <locust/sched.context.h>
#endif

TYPE_ID_DEF(context);

META_FIELDS_DEF(context) = {
    6, {
        META_FIELD_DEF(context, rsp, ptr),
		META_FIELD_DEF(context, func, ptr),
		META_FIELD_DEF(context, arg, ptr),
		META_FIELD_DEF(context, stack, ptr),
		META_FIELD_DEF(context, stack_len, uint64),
		META_FIELD_DEF(context, next, ptr),
    }
};

META_DEF(context, & META_FIELDS_OF(context), nil);

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

/*
 * Copyright (c) 2005 Russ Cox, MIT; see LIBTASK_COPYRIGHT
 * Altered by AndrewBC - 2012
 */

void context_init(context *self, uint64 stack_len, ptr stack_ptr, void (*func)(), ptr arg) {
    assert(self != nil);
    assert(stack_ptr != nil);
    assert(func != nil);

    debug("context_make: arg: %p", arg);
	self->func = (uintptr_t)func;
    self->arg = (uint64)arg;
	debug("context_make: arg again: %ld", self->arg);


    self->stack_len = stack_len;

	// stack grows down, so rsp must be on opposite side.
	self->stack = ((byte*)stack_ptr)+stack_len;

	// Also make room for the "pushq (uint64)func" and registers that has to be faked
	// so the first ret of the first call pops it off correctly
	self->rsp = (uint64*)self->stack;

	// Now we set up the return address fakery and registers
	*--self->rsp = 0x0; // bogus last rip?
	*--self->rsp = 0xDEADBEEF; // rbx
	*--self->rsp = 0xDEADBEEF; // r12
	*--self->rsp = 0xDEADBEEF; // r13
	*--self->rsp = 0xDEADBEEF; // r14
	*--self->rsp = 0xDEADBEEF; // r15
	*--self->rsp = 0xDEADBEEF; // rbp

	return;
}

void context_deinit(context *self) {
    assert(self != nil);
}
