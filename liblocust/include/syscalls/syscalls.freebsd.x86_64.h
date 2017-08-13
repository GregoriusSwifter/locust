#define SYS_call_0(ret, did_err, syscall_number) \
do { __asm__ __volatile__ (                    \
    "syscall;\n"                             \
    "xorb %[_did_err],%[_did_err];\n"         \
    "cmpq $-4095, %%rax;\n"                  \
    "jnae 1f;\n"                             \
    "not %[_did_err];\n"                      \
    "neg %%rax;\n"                           \
    "1:;\n"                                  \
    : "=a" (ret),                         \
      [_did_err] "=r" (did_err)          \
    : "0"(syscall_number)                 \
    : "cc", "rcx", "r11", "memory"        \
);} while(0)

#define SYS_call_1(ret, did_err, syscall_number, arg1) \
do { __asm__ __volatile__ (                          \
    "syscall;\n"                                   \
    "xorb %[_did_err],%[_did_err];\n"               \
    "cmpq $-4095, %%rax;\n"                        \
    "jnae 1f;\n"                                   \
    "not %[_did_err];\n"                           \
    "neg %%rax;\n"                                 \
    "1:;\n"                                        \
    : "=a" (ret),                               \
      [_did_err] "=r" (did_err)                \
    : "0"(syscall_number),                      \
      "D"(arg1)                                 \
    : "cc", "rcx", "r11", "memory"              \
);} while(0)

#define SYS_call_2(ret, did_err, syscall_number, arg1, arg2) \
do { __asm__ __volatile__ (                                \
    "syscall;\n"                                         \
    "xorb %[_did_err],%[_did_err];\n"                     \
    "cmpq $-4095, %%rax;\n"                              \
    "jnae 1f;\n"                                         \
    "not %[_did_err];\n"                                 \
    "neg %%rax;\n"                                       \
    "1:;\n"                                              \
    : "=a" (ret),                                     \
      [_did_err] "=r" (did_err)                      \
    : "0"(syscall_number),                            \
      "D"(arg1),                                      \
      "S"(arg2)                                       \
    : "cc", "rcx", "r11", "memory"                    \
);} while(0)

#define SYS_call_3(ret, did_err, syscall_number, arg1, arg2, arg3) \
do { __asm__ __volatile__ (                                      \
    "syscall;\n"                                               \
    "xorb %[_did_err],%[_did_err];\n"                           \
    "cmpq $-4095, %%rax;\n"                                    \
    "jnae 1f;\n"                                               \
    "not %[_did_err];\n"                                       \
    "neg %%rax;\n"                                             \
    "1:;\n"                                                    \
    : "=a" (ret),                                           \
      [_did_err] "=r" (did_err)                            \
    : "0"(syscall_number),                                  \
      "D"(arg1),                                            \
      "S"(arg2),                                            \
      "d"(arg3)                                             \
    : "cc", "rcx", "r11", "memory"                          \
);} while(0)

#define SYS_call_4(ret, did_err, syscall_number, arg1, arg2, arg3, arg4) \
do {                                                              \
    register __volatile__ long arg4_n __asm__ ("r10"); = ( arg4 )\
    __asm__ __volatile__ (                                        \
    "syscall;\n"                                                     \
    "xorb %[_did_err],%[_did_err];\n"                                 \
    "cmpq $-4095, %%rax;\n"                                          \
    "jnae 1f;\n"                                                     \
    "not %[_did_err];\n"                                             \
    "neg %%rax;\n"                                                   \
    "1:;\n"                                                          \
    : "=a" (ret),                                                 \
      [_did_err] "=r" (did_err)                                  \
    : "0"(syscall_number),                                        \
      "D"(arg1),                                                  \
      "S"(arg2),                                                  \
      "d"(arg3),                                                  \
      "r"(arg4_n)                                                 \
    : "cc", "rcx", "r11", "memory"                                \
);} while(0)

#define SYS_call_5(ret, did_err, syscall_number, arg1, arg2, arg3, arg4, arg5) \
do {                                                                    \
    register __volatile__ long arg4_n __asm__ ("r10") = ( arg4 );      \
    register __volatile__ long arg5_n __asm__ ("r8") = ( arg5 );       \
    __asm__ __volatile__ (                                              \
    "syscall;\n"                                                           \
    "xorb %[_did_err],%[_did_err];\n"                                       \
    "cmpq $-4095, %%rax;\n"                                                \
    "jnae 1f;\n"                                                           \
    "not %[_did_err];\n"                                                   \
    "neg %%rax;\n"                                                         \
    "1:;\n"                                                                \
    : "=a" (ret),                                                       \
      [_did_err] "=r" (did_err)                                        \
    : "0"(syscall_number),                                              \
      "D"(arg1),                                                        \
      "S"(arg2),                                                        \
      "d"(arg3),                                                        \
      "r"(arg4_n),                                                      \
      "r"(arg5_n)                                                        \
    : "cc", "rcx", "r11", "memory"                                      \
);} while(0)

#define SYS_call_6(ret, did_err, syscall_number, arg1, arg2, arg3, arg4, arg5, arg6) \
do {                                                                          \
    register __volatile__ long arg4_n __asm__ ("r10") = ( arg4 );             \
    register __volatile__ long arg5_n __asm__ ("r8") = ( arg5 );              \
    register __volatile__ long arg6_n __asm__ ("r9") = ( arg6 );              \
    __asm__ __volatile__ (                                                    \
    "syscall;\n"                                                                 \
    "xorb %[_did_err],%[_did_err];\n"                                             \
    "cmpq $-4095, %%rax;\n"                                                      \
    "jnae 1f;\n"                                                                 \
    "not %[_did_err];\n"                                                         \
    "neg %%rax;\n"                                                               \
    "1:;\n"                                                                      \
    : "=a" (ret),                                                             \
      [_did_err] "=r" (did_err)                                              \
    : "0"(syscall_number),                                                    \
      "D"(arg1),                                                              \
      "S"(arg2),                                                              \
      "d"(arg3),                                                              \
      "r"(arg4_n),                                                            \
      "r"(arg5_n),                                                             \
      "r"(arg6_n)                                                              \
    : "cc", "rcx", "r11", "memory"                                            \
);} while(0)