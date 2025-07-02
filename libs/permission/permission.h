// permission.h
#include <stdint.h>
#ifndef PERMISSION_H
#define PERMISSION_H

#define READ  0x4
#define WRITE 0x2
#define EXEC  0x1

typedef struct { // uint8_t limita entre 0 a 7
    uint8_t owner_perm;  // RWX
    uint8_t group_perm;
    uint8_t public_perm;
} Permission;

#endif
