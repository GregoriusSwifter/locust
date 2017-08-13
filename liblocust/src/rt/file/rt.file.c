#ifndef FILE_H
#include <locust/rt.file.h>
#endif

#include <unistd.h>
#include <errno.h>

TYPE_ID_DEF(file);

META_FIELDS_DEF(file) = {
    3, {
        {TYPE_ID_OF(ptr), sizeof(ptr), "handle"},
        {TYPE_ID_OF(string), sizeof(string), "name"},
        {TYPE_ID_OF(byte), sizeof(byte), "mode"},
    }
};

META_DEF(file, & META_FIELDS_OF(file), nil);

error file_err_not_file = { .id=1, .name="not_file", .info="File doesn't exist."};
error file_err_not_dir = { .id=2, .name="not_dir", .info="Directory doesn't exist."};
error file_err_unreadable = { .id=3, .name="unreadable", .info="File unreadable."};
error file_err_unwriteable = { .id=4, .name="unwriteable", .info="File unwriteable."};
error file_err_path_cycle = { .id=5, .name="path_cycle", .info="Path component resulted in traversal cycle."};
error file_err_long_name = { .id=6, .name="long_name", .info="Path component exceeds maximum length."};
error file_err_bad_perm = { .id=7, .name="bad_perm", .info="Bad permissions."};
error file_err_bad_mode = { .id=8, .name="bad_mode", .info="Bad mode."};
error file_err_unknown = { .id=9, .name="unknown", .info="Unknown error."};

// status queries
#define __file_readable(fileptr)  ((fileptr) != NULL && ((fileptr)->mode & READ) != 0)
#define __file_writeable(fileptr) ((fileptr) != NULL && ((fileptr)->mode & WRITE) != 0)

// file checks
error* file_exists(string filename) {
    int result = access(filename.bytes, F_OK);
    switch(result) {
        case EACCES:
            return &file_err_bad_perm;
        //case ELOOP:
        //    return &file_err_path_cycle;
        case ENAMETOOLONG:
            return &file_err_long_name;
        case ENOENT:
            return &file_err_not_file;
        case ENOTDIR:
            return &file_err_not_dir;
        default:
            return &file_err_unknown;
    }
}

error* file_check(string filename, byte mode) {

}

// file manipulations
error* file_create() {

}

error* file_open(file *f, string filename, byte mode) {

}

error* file_close(file *f) {

}


// reading functions
error* file_read(file *f, ptr str) {
    uint64 ret;
    bool did_err;
    SYSCALL_read(ret, did_err, fd, buf, nbyte)
}


// writing functions
error* file_write(file *f, array *a)  {
    uint64 ret;
    bool did_err;
    SYSCALL_write(ret, did_err, f->handle, a->items, a->length);
    return ret;
}