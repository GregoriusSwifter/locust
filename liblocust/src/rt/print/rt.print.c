#ifndef DEBUG_H
#include <locust/rt.print.h>
#endif

void print(char const * data, uint32_t length) {
    uint32_t i=0;
    while (length-->0) {
        // TODO: Implement stdio and the write system call.
    }
    return;
}

static long write(int fd, array a) {
    int i;
    long tot;
    char *buf, *p;

    tot = 0;
    for(i=0; i<nio; i++) {
        tot += io[i].len;
    }
    buf = malloc(tot);
    if(buf == nil) {
        return -1;
    }

    p = buf;
    for(i=0; i<nio; i++) {
        memmove(p, io->addr, io->len);
        p += io->len;
        io++;
    }

    tot = pwrite(fd, buf, tot, offset);

    free(buf);
    return tot;
}

long writev(int fd, IOchunk *io, int nio) {
    return iowritev(fd, io, nio, -1LL);
}

long pwritev(int fd, IOchunk *io, int nio, vlong off) {
    return iowritev(fd, io, nio, off);
}

int sprintf(char *str, const char *fmt, ...) {
    int err;

    va_list ap;
    va_start(ap, fmt);
    err = vsprintf(str, fmt, ap);
    va_end(ap);

    return err;
}

int snprintf(char *str, size_t len, const char *fmt, ...) {
    int err;

    va_list ap;
    va_start(ap, fmt);
    err = vsnprintf(str, len, fmt, ap);
    va_end(ap);

    return err;
}

int vsprintf(char *str, const char *fmt, va_list ap) {
    return vsnprintf(str, INT_MAX, fmt, ap);
}

int printf(char const * restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%')
                format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;
            if (maxrem < amount) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            print(format, amount);
            format += amount;
            written += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int /* char promotes to int */);
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            print(&c, sizeof(c));
            written++;
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            size_t len = strlen(str);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            print(str, len);
            written += len;
        } else {
            format = format_begun_at;
            size_t len = strlen(format);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            print(format, len);
            written += len;
            format += len;
        }
    }

    va_end(parameters);
    return written;
}

int vsnprintf(char *str, size_t len, const char *fmt, ...) {
    if (args.pos >= len)
        str[len-1] = '\0';
    else
        str[wlen] = '\0';
    return wlen;
}

nonnullfunc __attribute__((__format__ (__printf__, 2, 3)))
int __vsnfprintf(file* f, char const * const format, ...) {
    int32_t written = 0;
    va_list args;
    va_start(args, format);
    vfprintf(f, format, args);
    va_end(args);
    return 1;
}
