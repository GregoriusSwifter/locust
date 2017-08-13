#define SYS_call_0(ret, did_err, syscall_number) \
do { __asm__ __volatile__ (                    \
    "syscall"                             \
    "xor %[_did_err],%[_did_err]"         \
    "cmpq $-4095, %%rax"                  \
    "jnae 1f"                             \
    "not %[_did_err]"                     \
    "neg %%rax"                           \
    "1:"                                  \
    : "=a" (ret),                         \
      [_did_err] "=rm" (did_err)          \
    : "0"(syscall_number)                 \
    : "cc", "rcx", "r11", "memory"        \
)} while(0)

#define SYS_call_1(ret, did_err, syscall_number, arg1) \
do { __asm__ __volatile__ (                          \
    "syscall"                                   \
    "xor %[_did_err],%[_did_err]"               \
    "cmpq $-4095, %%rax"                        \
    "jnae 1f"                                   \
    "not %[_did_err]"                           \
    "neg %%rax"                                 \
    "1:"                                        \
    : "=a" (ret),                               \
      [_did_err] "=rm" (did_err)                \
    : "0"(syscall_number),                      \
      "D"(arg1)                                 \
    : "cc", "rcx", "r11", "memory"              \
)} while(0)

#define SYS_call_2(ret, did_err, syscall_number, arg1, arg2) \
do { __asm__ __volatile__ (                                \
    "syscall"                                         \
    "xor %[_did_err],%[_did_err]"                     \
    "cmpq $-4095, %%rax"                              \
    "jnae 1f"                                         \
    "not %[_did_err]"                                 \
    "neg %%rax"                                       \
    "1:"                                              \
    : "=a" (ret),                                     \
      [_did_err] "=rm" (did_err)                      \
    : "0"(syscall_number),                            \
      "D"(arg1),                                      \
      "S"(arg2)                                       \
    : "cc", "rcx", "r11", "memory"                    \
)} while(0)

#define SYS_call_3(ret, did_err, syscall_number, arg1, arg2, arg3) \
do { __asm__ __volatile__ (                                      \
    "syscall"                                               \
    "xor %[_did_err],%[_did_err]"                           \
    "cmpq $-4095, %%rax"                                    \
    "jnae 1f"                                               \
    "not %[_did_err]"                                       \
    "neg %%rax"                                             \
    "1:"                                                    \
    : "=a" (ret),                                           \
      [_did_err] "=rm" (did_err)                            \
    : "0"(syscall_number),                                  \
      "D"(arg1),                                            \
      "S"(arg2),                                            \
      "d"(arg3)                                             \
    : "cc", "rcx", "r11", "memory"                          \
)} while(0)

#define SYS_call_4(ret, did_err, syscall_number, arg1, arg2, arg3, arg4) \
do { __asm__ __volatile__ (                                            \
    "syscall"                                                     \
    "xor %[_did_err],%[_did_err]"                                 \
    "cmpq $-4095, %%rax"                                          \
    "jnae 1f"                                                     \
    "not %[_did_err]"                                             \
    "neg %%rax"                                                   \
    "1:"                                                          \
    : "=a" (ret),                                                 \
      [_did_err] "=rm" (did_err)                                  \
    : "0"(syscall_number),                                        \
      "D"(arg1),                                                  \
      "S"(arg2),                                                  \
      "d"(arg3),                                                  \
      "r10"(arg4)                                                 \
    : "cc", "rcx", "r11", "memory"                                \
)} while(0)

#define SYS_call_5(ret, did_err, syscall_number, arg1, arg2, arg3, arg4, arg5) \
do { __asm__ __volatile__ (                                                  \
    "syscall"                                                           \
    "xor %[_did_err],%[_did_err]"                                       \
    "cmpq $-4095, %%rax"                                                \
    "jnae 1f"                                                           \
    "not %[_did_err]"                                                   \
    "neg %%rax"                                                         \
    "1:"                                                                \
    : "=a" (ret),                                                       \
      [_did_err] "=rm" (did_err)                                        \
    : "0"(syscall_number),                                              \
      "D"(arg1),                                                        \
      "S"(arg2),                                                        \
      "d"(arg3),                                                        \
      "r10"(arg4),                                                      \
      "r8"(arg5)                                                        \
    : "cc", "rcx", "r11", "memory"                                      \
)} while(0)

#define SYS_call_6(ret, did_err, syscall_number, arg1, arg2, arg3, arg4, arg5, arg6) \
do { __asm__ __volatile__ (                                                        \
    "syscall"                                                                 \
    "xor %[_did_err],%[_did_err]"                                             \
    "cmpq $-4095, %%rax"                                                      \
    "jnae 1f"                                                                 \
    "not %[_did_err]"                                                         \
    "neg %%rax"                                                               \
    "1:"                                                                      \
    : "=a" (ret),                                                             \
      [_did_err] "=rm" (did_err)                                              \
    : "0"(syscall_number),                                                    \
      "D"(arg1),                                                              \
      "S"(arg2),                                                              \
      "d"(arg3),                                                              \
      "r10"(arg4),                                                            \
      "r8"(arg5),                                                             \
      "r9"(arg6)                                                              \
    : "cc", "rcx", "r11", "memory"                                            \
)} while(0)
