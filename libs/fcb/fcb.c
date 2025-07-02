#include <stdlib.h>
#include <string.h>
#include "fcb.h"

static int global_id = 1;

file_control_block* create_fcb(const char* name, file_type file_type, data_type data_type, Permission perm) {
    file_control_block* fcb = malloc(sizeof(file_control_block));
    if (!fcb) return NULL;

    strncpy(fcb->name, name, NAME_MAX);
    fcb->inode = global_id++;
    fcb->file_type = file_type;
    fcb->data_type = data_type;
    fcb->size = 0;
    fcb->created_at = fcb->modified_at = fcb->accessed_at = time(NULL);
    fcb->permissions = perm;
    return fcb;
}