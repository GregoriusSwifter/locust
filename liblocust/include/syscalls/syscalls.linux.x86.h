#define SYS_call_0(ret, did_err, syscall_number) \
do { __asm__ __volatile__ (                    \
    "int $0x80"                           \
    "xor %[_did_err],%[_did_err]"         \
    "cmpq $-4095, %%eax"                  \
    "jnae 1f"                             \
    "not %[_did_err]"                     \
    "neg %%eax"                           \
    "1:"                                  \
    : "=a" (result),                      \
      [_did_err] "=rm" (did_err)          \
    : "0"(syscall_number)                 \
    : "cc", "memory"                      \
)} while(0)

#define SYS_call_1(ret, did_err, syscall_number, arg1) \
do { __asm__ __volatile__ (                           \
    "int $0x80"                                  \
    "xor %[_did_err],%[_did_err]"         \
    "cmpq $-4095, %%eax"                  \
    "jnae 1f"                             \
    "not %[_did_err]"                     \
    "neg %%eax"                           \
    "1:"                                  \
    : "=a" (ret),                                \
      [_did_err] "=rm" (did_err)                 \
    : "0"(syscall_number),                       \
      "b"(arg1)                                  \
    : "cc", "memory"                             \
)} while(0)

#define SYS_call_2(ret, did_err, syscall_number, arg1, arg2) \
do { __asm__ __volatile__ (                                 \
    "int $0x80"                                        \
    "xor %[_did_err],%[_did_err]"         \
    "cmpq $-4095, %%eax"                  \
    "jnae 1f"                             \
    "not %[_did_err]"                     \
    "neg %%eax"                           \
    "1:"                                  \
    : "=a" (ret),                                      \
      [_did_err] "=rm" (did_err)                       \
    : "0"(syscall_number),                             \
      "b"(arg1),                                       \
      "c"(arg2)                                        \
    : "cc", "memory"                                   \
)} while(0)

#define SYS_call_3(ret, did_err, syscall_number, arg1, arg2, arg3) \
do { __asm__ __volatile__ (                                       \
    "int $0x80"                                              \
    "xor %[_did_err],%[_did_err]"         \
    "cmpq $-4095, %%eax"                  \
    "jnae 1f"                             \
    "not %[_did_err]"                     \
    "neg %%eax"                           \
    "1:"                                  \
    : "=a" (ret),                                            \
      [_did_err] "=rm" (did_err)                             \
    : "0"(syscall_number),                                   \
      "b"(arg1),                                             \
      "c"(arg2),                                             \
      "d"(arg3)                                              \
    : "cc", "memory"                                         \
)} while(0)

#define SYS_call_4(ret, did_err, syscall_number, arg1, arg2, arg3, arg4) \
do { __asm__ __volatile__ (                                             \
    "int $0x80"                                                    \
    "xor %[_did_err],%[_did_err]"         \
    "cmpq $-4095, %%eax"                  \
    "jnae 1f"                             \
    "not %[_did_err]"                     \
    "neg %%eax"                           \
    "1:"                                  \
    : "=a" (ret),                                                  \
      [_did_err] "=rm" (did_err)                                   \
    : "0"(syscall_number),                                         \
      "b"(arg1),                                                   \
      "c"(arg2),                                                   \
      "d"(arg3),                                                   \
      "S"(arg4)                                                    \
    : "cc", "memory"                                               \
)} while(0)

#define SYS_call_5(ret, did_err, syscall_number, arg1, arg2, arg3, arg4, arg5) \
do { __asm__ __volatile__ (                                                   \
    "int $0x80"                                                          \
    "xor %[_did_err],%[_did_err]"         \
    "cmpq $-4095, %%eax"                  \
    "jnae 1f"                             \
    "not %[_did_err]"                     \
    "neg %%eax"                           \
    "1:"                                  \
    : "=a" (ret),                                                        \
      [_did_err] "=rm" (did_err)                                         \
    : "0"(syscall_number),                                               \
      "b"(arg1),                                                         \
      "c"(arg2),                                                         \
      "d"(arg3),                                                         \
      "S"(arg4),                                                         \
      "D"(arg5)                                                          \
    : "cc", "memory"                                                     \
)} while(0)

#define SYS_call_6(ret, did_err, syscall_number, arg1, arg2, arg3, arg4, arg5, arg6) \
do { __asm__ __volatile__ (                                                         \
    "mov %[arg_6],%%ebp"                                                       \
    "int $0x80"                                                                \
    "xor %[_did_err],%[_did_err]"         \
    "cmpq $-4095, %%eax"                  \
    "jnae 1f"                             \
    "not %[_did_err]"                     \
    "neg %%eax"                           \
    "1:"                                  \
    : "=a" (ret),                                                              \
      [_did_err] "=rm" (did_err)                                               \
    : "0"(syscall_number),                                                     \
      "b"(arg1),                                                               \
      "c"(arg2),                                                               \
      "d"(arg3),                                                               \
      "S"(arg4),                                                               \
      "D"(arg5),                                                               \
      [arg_6] "rm" (arg6)                                                      \
    : "cc", "ebp", "memory"                                                    \
)} while(0)
