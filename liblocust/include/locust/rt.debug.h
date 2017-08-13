#define DEBUG_H

#define __STRINGIFY__(A) #A
#define STRINGIFY(A) __STRINGIFY__(A)

#define log_quiet(...) (__vsnfprintf(stderr, __VA_ARGS__))
#define die(...)   (__vsnfprintf(stderr, "[DIE]: (%s:%d in %s) ", __func__, __FILE__, __LINE__); __vsnfprintf(stderr, __VA_ARGS__); abort(); __builtin_unreachable())
#define error(...) (__vsnfprintf(stderr, "[ERR]: (%s:%d in %s) ", __func__, __FILE__, __LINE__); __vsnfprintf(stderr, __VA_ARGS__)
#define warn(...)  (__vsnfprintf(stderr, "[WRN]: (%s:%d in %s) ", __func__, __FILE__, __LINE__); __vsnfprintf(stderr, __VA_ARGS__)
#define info(...)  (__vsnfprintf(stderr, "[INF]: (%s:%d in %s) ", __func__, __FILE__, __LINE__); __vsnfprintf(stderr, __VA_ARGS__)
#define debug(...) (__vsnfprintf(stderr, "[DBG]: (%s:%d in %s) ", __func__, __FILE__, __LINE__); __vsnfprintf(stderr, __VA_ARGS__)

#define assert(EXPR) ((EXPR) ? (void)0 : (void) error(STRINGIFY(EXPR)))
#define assert_static(EXPR) (_Static_assert( (EXPR), "[ERR]: (" __FILE__ ":" STRINGIFY(__LINE__) " in " STRINGIFY(__func__) ") " STRINGIFY((EXPR))))

#ifdef NDEBUG
#define debug(...) ((void)0)
#endif

#ifdef PRINT_H
#include <rt.print.h>
#endif
