#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "filesystem.h"

extern User current_user;

static Permission default_dir_perm = { .owner_perm = 7, .group_perm = 5, .public_perm = 5 };
static Permission default_file_perm = { .owner_perm = 6, .group_perm = 4, .public_perm = 4 };

static inode* create_inode(file_type type, const char* name, Permission perm) {
    inode* node = malloc(sizeof(inode));
    if (!node) return NULL;

    memset(node, 0, sizeof(inode));
    strncpy(node->fcb.name, name, NAME_MAX);
    node->fcb.inode = rand();  // Inode único (ou use contador global se desejar)
    node->fcb.file_type = type;
    node->fcb.data_type = CHARACTER;
    node->fcb.size = 0;
    time_t now = time(NULL);
    node->fcb.created_at = node->fcb.modified_at = node->fcb.accessed_at = now;
    node->fcb.permissions = perm;
    return node;
}

static inode* find_entry(inode* cwd, const char* name, int* index_out) {
    if (!cwd || cwd->fcb.file_type != DIRECTORY) return NULL;
    for (int i = 0; i < cwd->dir.entry_count; ++i) {
        if (strcmp(cwd->dir.entries[i].file->fcb.name, name) == 0) {
            if (index_out) *index_out = i;
            return cwd->dir.entries[i].file;
        }
    }
    return NULL;
}

void help(void) {
    printf("Comandos disponíveis:\n");
    printf("mkdir <nome>\ncd <nome>\nls\nls -l\ntouch <nome>\ncat <nome>\necho <conteúdo> > <arquivo>\n");
    printf("cp <origem> <destino>\nmv <origem> <destino>\nrm <nome>\nchmod <perm> <nome>\nstat <nome>\nclear\n");
}

void mkdir(inode* cwd, const char* name) {
    if (find_entry(cwd, name, NULL)) {
        printf("mkdir: '%s' já existe\n", name);
        return;
    }
    if (cwd->dir.entry_count >= MAX_ENTRIES) {
        printf("mkdir: limite de entradas atingido\n");
        return;
    }

    inode* dir = create_inode(DIRECTORY, name, default_dir_perm);
    dir->parent = cwd;
    cwd->dir.entries[cwd->dir.entry_count++] = (dir_entry){ .inode_number = dir->fcb.inode, .file = dir };
}

inode* cd(inode* cwd, const char* name) {
    if (strcmp(name, "..") == 0) {
        if (cwd->parent != NULL) {
            return cwd->parent;
        }

        printf("cd: não há diretório pai\n");
        return cwd;
    }

    inode* entry = find_entry(cwd, name, NULL);
    if (!entry || entry->fcb.file_type != DIRECTORY) {
        printf("cd: '%s' não é um diretório\n", name);
        return cwd;
    }
    return entry;
}

void ls(inode* cwd, const char* path) {
    inode* target = path ? resolve_path(cwd, path) : cwd;

    if (!target) {
        printf("ls: caminho '%s' não encontrado\n", path);
        return;
    }

    if (target->fcb.file_type != DIRECTORY) {
        printf("ls: '%s' não é um diretório\n", path);
        return;
    }

    for (int i = 0; i < target->dir.entry_count; ++i) {
        printf("%s\t", target->dir.entries[i].file->fcb.name);
    }
    printf("\n");
}


void ls_l(inode* cwd, const char* name) {
    (void)name;
    for (int i = 0; i < cwd->dir.entry_count; ++i) {
        inode* entry = cwd->dir.entries[i].file;
        char type = entry->fcb.file_type == DIRECTORY ? 'd' : '-';
        printf("%c", type);
        printf("%c%c%c ",
               (entry->fcb.permissions.owner_perm & READ) ? 'r' : '-',
               (entry->fcb.permissions.owner_perm & WRITE) ? 'w' : '-',
               (entry->fcb.permissions.owner_perm & EXEC) ? 'x' : '-');
        printf("%s\t%d bytes\t%s", entry->fcb.name, entry->fcb.size, ctime(&entry->fcb.modified_at));
    }
}

void touch(inode* cwd, const char* name) {
    if (find_entry(cwd, name, NULL)) return;
    if (cwd->dir.entry_count >= MAX_ENTRIES) return;

    inode* file = create_inode(REGULAR, name, default_file_perm);
    cwd->dir.entries[cwd->dir.entry_count++] = (dir_entry){ .inode_number = file->fcb.inode, .file = file };
}

void cat(inode* cwd, const char* name) {
    inode* file = find_entry(cwd, name, NULL);
    if (!file || file->fcb.file_type != REGULAR) {
        printf("cat: '%s' não encontrado ou não é um arquivo\n", name);
        return;
    }
    printf("%s\n", file->file.content);
}

void echo(inode* cwd, const char* content, const char* filename) {
    inode* file = find_entry(cwd, filename, NULL);
    if (!file) {
        if (cwd->dir.entry_count >= MAX_ENTRIES) return;
        file = create_inode(REGULAR, filename, default_file_perm);
        cwd->dir.entries[cwd->dir.entry_count++] = (dir_entry){ .inode_number = file->fcb.inode, .file = file };
    }
    strncpy(file->file.content, content, CONTENT_MAX);
    file->fcb.size = strlen(content);
    file->fcb.modified_at = time(NULL);
}

void cp(inode* cwd, const char* src, const char* dest) {
    inode* source = find_entry(cwd, src, NULL);
    if (!source || source->fcb.file_type != REGULAR) {
        printf("cp: '%s' não encontrado ou não é um arquivo\n", src);
        return;
    }

    inode* target = find_entry(cwd, dest, NULL);

    if (target) {
        if (target->fcb.file_type != REGULAR) {
            printf("cp: '%s' já existe e não é um arquivo\n", dest);
            return;
        }
        // Overwrite
        strncpy(target->file.content, source->file.content, CONTENT_MAX);
        target->fcb.size = source->fcb.size;
        target->fcb.modified_at = time(NULL);
    } else {
        if (cwd->dir.entry_count >= MAX_ENTRIES) return;
        inode* copy = create_inode(REGULAR, dest, source->fcb.permissions);
        strncpy(copy->file.content, source->file.content, CONTENT_MAX);
        copy->fcb.size = source->fcb.size;
        cwd->dir.entries[cwd->dir.entry_count++] = (dir_entry){ .inode_number = copy->fcb.inode, .file = copy };
    }
}


void mv(inode* cwd, const char* src, const char* dest) {
    int src_index = -1;
    inode* source = find_entry(cwd, src, &src_index);
    if (!source) {
        printf("mv: '%s' não encontrado\n", src);
        return;
    }

    inode* target = find_entry(cwd, dest, NULL);

    if (target && target->fcb.file_type == DIRECTORY) {
        // move src into dest directory
        if (target->dir.entry_count >= MAX_ENTRIES) {
            printf("mv: destino '%s' cheio\n", dest);
            return;
        }

        // Remove from current directory
        for (int i = src_index; i < cwd->dir.entry_count - 1; ++i) {
            cwd->dir.entries[i] = cwd->dir.entries[i + 1];
        }
        cwd->dir.entry_count--;

        // Add to destination directory
        source->parent = target;
        target->dir.entries[target->dir.entry_count++] = (dir_entry){
            .inode_number = source->fcb.inode,
            .file = source
        };
    } else {
        // Simple rename within current directory
        strncpy(source->fcb.name, dest, NAME_MAX);
    }
}


void rm(inode* cwd, const char* name) {
    int index = -1;
    inode* file = find_entry(cwd, name, &index);
    if (!file) {
        printf("rm: '%s' não encontrado\n", name);
        return;
    }

    free(file);
    for (int i = index; i < cwd->dir.entry_count - 1; ++i) {
        cwd->dir.entries[i] = cwd->dir.entries[i + 1];
    }
    cwd->dir.entry_count--;
}

void chmod_cmd(inode* cwd, const char* name, int mode) {
    inode* file = find_entry(cwd, name, NULL);
    if (!file) {
        printf("chmod: '%s' não encontrado\n", name);
        return;
    }
    file->fcb.permissions.owner_perm = (mode >> 6) & 7;
    file->fcb.permissions.group_perm = (mode >> 3) & 7;
    file->fcb.permissions.public_perm = mode & 7;
}

void stat_cmd(inode* cwd, const char* name) {
    inode* file = find_entry(cwd, name, NULL);
    if (!file) {
        printf("stat: '%s' não encontrado\n", name);
        return;
    }

    printf("Nome: %s\n", file->fcb.name);
    printf("Tipo: %s\n", file->fcb.file_type == REGULAR ? "Arquivo" : "Diretório");
    printf("Tamanho: %d bytes\n", file->fcb.size);
    printf("Permissões (owner): %c%c%c\n",
           (file->fcb.permissions.owner_perm & READ) ? 'r' : '-',
           (file->fcb.permissions.owner_perm & WRITE) ? 'w' : '-',
           (file->fcb.permissions.owner_perm & EXEC) ? 'x' : '-');
    printf("Criado em: %s", ctime(&file->fcb.created_at));
    printf("Modificado em: %s", ctime(&file->fcb.modified_at));
    printf("Acessado em: %s", ctime(&file->fcb.accessed_at));
}

inode* resolve_path(inode* cwd, const char* path) {
    if (!path || strlen(path) == 0) return cwd;

    char path_copy[1024];
    strncpy(path_copy, path, sizeof(path_copy));
    char* token = strtok(path_copy, "/");

    inode* current = cwd;

    while (token) {
        inode* next = NULL;
        for (int i = 0; i < current->dir.entry_count; ++i) {
            if (strcmp(current->dir.entries[i].file->fcb.name, token) == 0) {
                next = current->dir.entries[i].file;
                break;
            }
        }
        if (!next || next->fcb.file_type != DIRECTORY) return NULL;
        current = next;
        token = strtok(NULL, "/");
    }

    return current;
}
