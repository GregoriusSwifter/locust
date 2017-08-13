.text
/*
void context_swap(stack_ptr *old_sp, stack_ptr *new_sp);
 - assumes that -fomit-frame-pointer was specified.
*/

.global context_swap
context_swap:
    // save context from machine registers to old_sp
    popq %rcx
    movq %rcx, 8(%rdi) // save return address
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    pushq %rbp
    movq %rsp, (%rdi)

    // load context from new_sp to machine registers
    movq (%rsi), %rsp
    popq %rbp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx

    // restore arg to its set value
    movq 16(%rsi), %rdi

    // Set up %rip and then go.
    pushq 8(%rsi)
    ret
.size context_swap, . - context_swap
.type context_swap, @function
