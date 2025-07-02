#ifndef FCB_H
#define FCB_H

#include <time.h>
#include "../permission/permission.h"

#define NAME_MAX 64
#define CONTENT_MAX 1024

typedef enum {
    REGULAR,
    DIRECTORY
} file_type;

typedef enum {
    NUMERIC,
    CHARACTER,
    BINARY,
    PROGRAM
} data_type;

typedef struct file_control_block {
    char name[NAME_MAX];
    int inode;
    file_type file_type;
    data_type data_type;
    int size;
    time_t created_at;
    time_t modified_at;
    time_t accessed_at;
    Permission permissions;

} file_control_block;

#endif // FCB_H
