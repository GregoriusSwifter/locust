#define SYS_call_0(ret, did_err, syscall_number) \
do { __asm__ __volatile__ (                       \
    "push %%ax"                                  \
    "int $0x80"                           \
    "sbb %[_did_err],%[_did_err]"         \
    "add $4,%%esp"                        \
    : "=a" (ret),                         \
      [_did_err] "=rm" (did_err)          \
    : "0"(syscall_number)                 \
    : "cc", "memory"        \
)} while(0)

#define SYS_call_1(ret, did_err, syscall_number, arg1) \
do { __asm__ __volatile__ (                          \
    "push %[arg_1]"                             \
    "push %%ax"                                 \
    "int $0x80"                                 \
    "sbb %[_did_err],%[_did_err]"               \
    "add $8,%%esp"                              \
    : "=a" (ret),                               \
      [_did_err] "=rm" (did_err)                \
    : "0"(syscall_number),                      \
      [arg_1] "rm" (arg1)                       \
    : "cc", "memory"                            \
)} while(0)

#define SYS_call_2(ret, did_err, syscall_number, arg1, arg2) \
do { __asm__ __volatile__ (                                \
    "push %[arg_2]"                                   \
    "push %[arg_1]"                                   \
    "push %%ax"                                       \
    "int $0x80"                                       \
    "sbb %[_did_err],%[_did_err]"                     \
    "add $12,%%esp"                                   \
    : "=a" (ret),                                     \
      [_did_err] "=rm" (did_err)                      \
    : "0"(syscall_number),                            \
      [arg_1] "rm" (arg1)                             \
      [arg_2] "rm" (arg2)                             \
    : "cc", "memory"                                  \
)} while(0)

#define SYS_call_3(ret, did_err, syscall_number, arg1, arg2, arg3) \
do { __asm__ __volatile__ (                                      \
    "push %[arg_3]"                                         \
    "push %[arg_2]"                                         \
    "push %[arg_1]"                                         \
    "push %%ax"                                             \
    "int $0x80"                                             \
    "sbb %[_did_err],%[_did_err]"                           \
    "add $16,%%esp"                                         \
    : "=a" (ret),                                           \
      [_did_err] "=rm" (did_err)                            \
    : "0"(syscall_number),                                  \
      [arg_1] "rm" (arg1),                                  \
      [arg_2] "rm" (arg2),                                  \
      [arg_3] "rm" (arg3),                                  \
    : "cc", "memory"                                        \
)} while(0)

#define SYS_call_4(ret, did_err, syscall_number, arg1, arg2, arg3, arg4) \
do { __asm__ __volatile__ (                                            \
    "push %[arg_4]"                                               \
    "push %[arg_3]"                                               \
    "push %[arg_2]"                                               \
    "push %[arg_1]"                                               \
    "push %%ax"                                                   \
    "int $0x80"                                                   \
    "sbb %[_did_err],%[_did_err]"                                 \
    "add $20,%%esp"                                               \
    : "=a" (ret),                                                 \
      [_did_err] "=rm" (did_err)                                  \
    : "0"(syscall_number),                                        \
      [arg_1] "rm" (arg1),                                        \
      [arg_2] "rm" (arg2),                                        \
      [arg_3] "rm" (arg3),                                        \
      [arg_4] "rm" (arg4),                                        \
    : "cc", "memory"                                              \
)} while(0)

#define SYS_call_5(ret, did_err, syscall_number, arg1, arg2, arg3, arg4, arg5) \
do { __asm__ __volatile__ (                                                  \
    "push %[arg_5]"                                                     \
    "push %[arg_4]"                                                     \
    "push %[arg_3]"                                                     \
    "push %[arg_2]"                                                     \
    "push %[arg_1]"                                                     \
    "push %%ax"                                                         \
    "int $0x80"                                                         \
    "sbb %[_did_err],%[_did_err]"                                       \
    "add $24,%%esp"                                                     \
    : "=a" (ret),                                                       \
      [_did_err] "=rm" (did_err)                                        \
    : "0"(syscall_number),                                              \
      [arg_1] "rm" (arg1),                                              \
      [arg_2] "rm" (arg2),                                              \
      [arg_3] "rm" (arg3),                                              \
      [arg_4] "rm" (arg4),                                              \
      [arg_5] "rm" (arg5),                                              \
    : "cc", "memory"                                                    \
)} while(0)

#define SYS_call_6(ret, did_err, syscall_number, arg1, arg2, arg3, arg4, arg5, arg6) \
do { __asm__ __volatile__ (                                                        \
    "push %[arg_6];"                                                           \
    "push %[arg_5];"                                                           \
    "push %[arg_4];"                                                           \
    "push %[arg_3];"                                                           \
    "push %[arg_2];"                                                           \
    "push %[arg_1];"                                                           \
    "push %%ax;"                                                               \
    "int $0x80;"                                                               \
    "sbb %[_did_err],%[_did_err];"                                             \
    "add $28,%%esp;"                                                           \
    : "=a" (ret),                                                             \
      [_did_err] "=rm" (did_err)                                              \
    : "0"(syscall_number),                                                    \
      [arg_1] "rm" (arg1),                                                    \
      [arg_2] "rm" (arg2),                                                    \
      [arg_3] "rm" (arg3),                                                    \
      [arg_4] "rm" (arg4),                                                    \
      [arg_5] "rm" (arg5),                                                    \
      [arg_6] "rm" (arg6),                                                    \
    : "cc", "memory"                                                          \
)} while(0)