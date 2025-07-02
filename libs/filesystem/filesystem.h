#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../fcb/fcb.h"
#include "../user/user.h"

#define MAX_ENTRIES 50

typedef struct inode inode;

typedef struct dir_entry {
    int inode_number;
    inode *file;
} dir_entry;

struct inode {
    file_control_block fcb;
    struct inode *parent;

    union {
        struct {
            char content[CONTENT_MAX];
        } file;

        struct {
            dir_entry entries[MAX_ENTRIES];
            int entry_count;
        } dir;
    };
};

// Comandos
void help(void);
void mkdir(inode* cwd, const char* name);
inode* cd(inode* cwd, const char* name);
void ls(inode* cwd, const char* path);
void ls_l(inode* cwd, const char* path);
void touch(inode* cwd, const char* name);
void cat(inode* cwd, const char* name);
void echo(inode* cwd, const char* content, const char* filename);
void cp(inode* cwd, const char* src, const char* dest);
void mv(inode* cwd, const char* src, const char* dest);
void rm(inode* cwd, const char* name);
void chmod_cmd(inode* cwd, const char* name, int mode);
void stat_cmd(inode* cwd, const char* name);

inode* resolve_path(inode* cwd, const char* path);

#endif // FILESYSTEM_H
