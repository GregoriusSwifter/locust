#define PRINT_H

__attribute__((__format__ (__printf__, 2, 3)))
int __vsnfprintf(file *f, char const * const format, ...);