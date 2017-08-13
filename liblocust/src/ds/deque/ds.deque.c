#ifndef DEQUE_H
#include <locust/ds.deque.h>
#endif

META_FIELDS_DEF(deque) = {
    4, {
        META_FIELD_DEF(deque, len, uint64),
        META_FIELD_DEF(deque, head, ptr),
        META_FIELD_DEF(deque, tail, ptr),
        META_FIELD_DEF(deque, items_desc, ptr),
    }
};

META_DEF(deque);

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef ATOMIC_H
#include <locust/sched.atomic.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

nonnullfunc void
deque_init(deque *self, const meta *type) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
#endif
    self->items_desc = type;
    self->len = 0;

    // Set up a token object to always have at least 1 item in deque.
    // Allows us to not worry about null pointer head and tail ever.
    // Good for parallelism.
    self->head = self->tail = mem_get(sizeof(deque_node)+type->size);
    assert(self->head != nil);
    mem_set(self->head, sizeof(deque_node)+type->size, 0);
}

nonnullfunc void
deque_deinit(deque *self) {
#if !nonnullfunc_on
    assert(self != nil);
#endif
    deque_node* curr = self->head;
    if (curr == nil) {
        return;
    }
    deque_node* next = curr->next;
    while (curr != nil) {
        mem_free(curr);
        curr = next;
        next = curr->next;
    }
}

nonnullfunc void
deque_back_push(deque *self, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    TYPE_CHECK(self->items_desc, type);
    deque_node *n = mem_get(sizeof(deque_node)+type->size);
    assert(n != nil);

    mem_copy(item, &n->content, type->size);
    n->next = nil;

	self->tail->next = n;
	self->tail = n;
    ++self->len;
}

nonnullfunc void
deque_front_push(deque *self, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    TYPE_CHECK(self->items_desc, type);
    deque_node *n = mem_get(sizeof(deque_node)+type->size);
    assert(n != nil);

    mem_copy(item, &n->content, type->size);
    n->next = nil;

    // the ->next Keeps the token object at the real head of the deque
    deque_node *head = atomic_swap(self->head->next, n);
    if (head) {
        n->next = head;
    }
    ++self->len;
}

nonnullfunc bool
deque_front_pop(deque *self, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    TYPE_CHECK(self->items_desc, type);
    deque_node* next = self->head->next;

    if (next) {
        self->head->next = next->next;
		if ((uintptr_t)self->tail == (uintptr_t)next) {
			self->tail = self->head;
		}
        mem_copy(&next->content, item, type->size);
        --self->len;
        mem_free(next);
        return true;
    }

    return false;
}

nonnullfunc bool
deque_back_pop(deque *self, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    TYPE_CHECK(self->items_desc, type);
    deque_node* tail = self->tail;
    deque_node* next = tail->next;

    if (next) {
        self->tail = next;
        mem_copy(&next->content, item, type->size);
        --self->len;
        mem_free(tail);
        return true;
    }

    return false;
}


