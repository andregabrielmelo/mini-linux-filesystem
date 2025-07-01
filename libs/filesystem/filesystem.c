#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "filesystem.h"

#define S_IFMT 0xF000 // Máscara para extrair o tipo do arquivo
#define S_IFDIR 0x4000 // diretório
#define S_IFREG 0x8000 // arquivo

ext4_inode* inode_table[MAX_INODES] = {NULL};
ext4_dir_entry* dir_entries[MAX_INODES] = {NULL}; // conteúdo do diretório
int inode_counter = 1;

// Verifica se o inode representa um diretório
int is_directory(ext4_inode* inode) {
    return (inode->mode & S_IFMT) == S_IFDIR;
}

// Verifica se o inode representa um arquivo regular
int is_regular_file(ext4_inode* inode) {
    return (inode->mode & S_IFMT) == S_IFREG;
}

int allocate_inode() {
    if (inode_counter >= MAX_INODES) return 0;
    return inode_counter++;
}

void print_permissions(int perm) {
    char rwx[] = {'r', 'w', 'x'};
    for (int i = 6; i >= 0; i -= 3) {
        int val = (perm >> i) & 0x7;
        for (int j = 2; j >= 0; j--) {
            printf("%c", (val & (1 << j)) ? rwx[2-j] : '-');
        }
    }
}

int check_permission(int perm, char mode, int uid, int gid) {
    // Placeholder: sem owner/group implementado ainda
    int val = (perm >> 6) & 0x7; // owner permissions
    if (mode == 'r') return val & 0x4;
    if (mode == 'w') return val & 0x2;
    if (mode == 'x') return val & 0x1;
    return 0;
}

ext4_inode* create_inode(ext4_file_type type, int32_t mode) {
    int index = allocate_inode();
    if (!index) return NULL;

    ext4_inode* node = calloc(1, sizeof(ext4_inode));
    node->mode = mode;
    node->uid = 0;
    node->gid = 0;
    node->size = 0;
    node->ctime = node->mtime = node->atime = time(NULL);
    node->links_count = 1;

    inode_table[index] = node;

    if (type == EXT4_FT_DIR) {
        dir_entries[index] = calloc(10, sizeof(ext4_dir_entry)); // limitado para simplicidade
    }

    return inode_table[index];
}

int add_dir_entry(ext4_inode* dir, const char* name, ext4_file_type type, ext4_inode* file) {
    int dir_index = -1;
    for (int i = 1; i < MAX_INODES; i++) {
        if (inode_table[i] == dir) {
            dir_index = i;
            break;
        }
    }
    if (dir_index < 0) return -1;

    ext4_dir_entry* entries = dir_entries[dir_index];
    for (int i = 0; i < 10; i++) {
        if (entries[i].inode == 0) {
            entries[i].inode = (int32_t)(file - inode_table[0]);
            entries[i].file_type = type;
            entries[i].name_len = strlen(name);
            entries[i].size = sizeof(ext4_dir_entry);
            strncpy(entries[i].name, name, EXT4_NAME_LEN);
            return 0;
        }
    }
    return -1;
}

void mkdir(ext4_inode* cwd, const char* dirname) {
    ext4_inode* dir = create_inode(EXT4_FT_DIR, 0755);
    if (!dir) {
        printf("Erro ao criar diretório.\n");
        return;
    }
    add_dir_entry(cwd, dirname, EXT4_FT_DIR, dir);
}

void touch(ext4_inode* cwd, const char* filename) {
    ext4_inode* file = create_inode(EXT4_FT_REG_FILE, 0644);
    if (!file) {
        printf("Erro ao criar arquivo.\n");
        return;
    }
    add_dir_entry(cwd, filename, EXT4_FT_REG_FILE, file);
}

ext4_inode* lookup(ext4_inode* cwd, const char* name) {
    int dir_index = -1;
    for (int i = 1; i < MAX_INODES; i++) {
        if (inode_table[i] == cwd) {
            dir_index = i;
            break;
        }
    }
    if (dir_index < 0) return NULL;

    ext4_dir_entry* entries = dir_entries[dir_index];
    for (int i = 0; i < 10; i++) {
        if (entries[i].inode != 0 && strcmp(entries[i].name, name) == 0) {
            return inode_table[entries[i].inode];
        }
    }
    return NULL;
}

void ls(ext4_inode* cwd, const char* filename) {
    int dir_index = -1;
    for (int i = 1; i < MAX_INODES; i++) {
        if (inode_table[i] == cwd) {
            dir_index = i;
            break;
        }
    }

    ext4_dir_entry* entries = dir_entries[dir_index];
    for (int i = 0; i < 10; i++) {
        if (entries[i].inode != 0) {
            printf("%s  ", entries[i].name);
        }
    }
    printf("\n");
}

void print_permissions(int32_t mode) {
    char perms[10] = "---------";

    int shifts[] = {MODE_OWNER_SHIFT, MODE_GROUP_SHIFT, MODE_OTHER_SHIFT};
    for (int i = 0; i < 3; i++) {
        int m = (mode >> shifts[i]) & 0x7;
        if (m & MODE_READ) perms[i * 3 + 0] = 'r';
        if (m & MODE_WRITE) perms[i * 3 + 1] = 'w';
        if (m & MODE_EXEC) perms[i * 3 + 2] = 'x';
    }

    printf("%s", perms);
}

void ls_l(ext4_inode* cwd, const char* filename) {
    int dir_index = -1;
    for (int i = 1; i < MAX_INODES; i++) {
        if (inode_table[i] == cwd) {
            dir_index = i;
            break;
        }
    }

    ext4_dir_entry* entries = dir_entries[dir_index];
    for (int i = 0; i < 10; i++) {
        if (entries[i].inode != 0) {
            ext4_inode* node = inode_table[entries[i].inode];
            print_permissions(node->mode);
            printf(" %3d %s\n", node->links_count, entries[i].name);
        }
    }
}

void cat(ext4_inode* cwd, const char* filename) {
    ext4_inode* file = lookup(cwd, filename);
    if (!file) {
        printf("Arquivo não encontrado\n");
        return;
    }
    printf("Conteúdo do arquivo '%s' [inode %ld]:\n", filename, file - inode_table[0]);
    // Conteúdo simulado: nenhum real ainda
}

ext4_inode* cd(ext4_inode* cwd, const char* dirname) {
    ext4_inode* target = lookup(*cwd, dirname);
    if (!target) {
        printf("Diretório não encontrado.\n");
        return cwd;
    }
    if (dir_entries[target - inode_table[0] == NULL) {
        printf("Não é um diretório.\n");
        return cwd;
    }
    return target;
}

void echo(ext4_inode* cwd, const char* content, const char* dest_filename) {
    ext4_inode* file = lookup(cwd, dest_filename);
    if (!file) {
        touch(cwd, dest_filename);
        file = lookup(cwd, dest_filename);
    }
    file->size = strlen(content);
    file->mtime = time(NULL);
    // Em uma simulação real, deveríamos armazenar o conteúdo nos blocos
    printf("Conteúdo '%s' gravado no arquivo '%s'.\n", content, dest_filename);
}

void mv(ext4_inode* cwd, const char* src, const char* dest) {
    int dir_index = -1;
    for (int i = 1; i < MAX_INODES; i++) {
        if (inode_table[i] == cwd) {
            dir_index = i;
            break;
        }
    }

    if (dir_index < 0) return;

    ext4_dir_entry* entries = dir_entries[dir_index];
    for (int i = 0; i < 10; i++) {
        if (entries[i].inode && strcmp(entries[i].name, src) == 0) {
            strncpy(entries[i].name, dest, EXT4_NAME_LEN);
            entries[i].name_len = strlen(dest);
            printf("'%s' renomeado para '%s'.\n", src, dest);
            return;
        }
    }

    printf("Arquivo/diretório '%s' não encontrado.\n", src);
}

void rm(ext4_inode* cwd, const char* name) {
    int dir_index = -1;
    for (int i = 1; i < MAX_INODES; i++) {
        if (inode_table[i] == cwd) {
            dir_index = i;
            break;
        }
    }

    if (dir_index < 0) return;

    ext4_dir_entry* entries = dir_entries[dir_index];
    for (int i = 0; i < 10; i++) {
        if (entries[i].inode && strcmp(entries[i].name, name) == 0) {
            int inode_id = entries[i].inode;
            free(inode_table[inode_id]);
            inode_table[inode_id] = NULL;
            if (dir_entries[inode_id]) {
                free(dir_entries[inode_id]);
                dir_entries[inode_id] = NULL;
            }
            entries[i].inode = 0;
            printf("Removido: %s\n", name);
            return;
        }
    }

    printf("Arquivo/diretório '%s' não encontrado.\n", name);
}

void chmod(ext4_inode* cwd, const char* filename, mode_t mode) {
    ext4_inode* file = lookup(cwd, filename);
    if (!file) {
        printf("Arquivo não encontrado.\n");
        return;
    }
    file->mode = mode;
    printf("Permissões alteradas.\n");
}

void stat(ext4_inode* cwd, const char* filename) {
    ext4_inode* file = lookup(cwd, filename);
    if (!file) {
        printf("Arquivo não encontrado.\n");
        return;
    }

    printf("Inode: %ld\n", file - inode_table[0]);
    printf("Tamanho: %d bytes\n", file->size);
    printf("Permissões: "); print_permissions(file->mode); printf("\n");
    printf("Links: %d\n", file->links_count);
    printf("UID: %d, GID: %d\n", file->uid, file->gid);
    printf("Modificado em: %s", ctime(&file->mtime));
}

void help() {
    printf("Comandos disponíveis:\n");
    printf("  mkdir <dir>: cria um diretório\n");
    printf("  cd <dir>: muda o diretório atual\n");
    printf("  ls -l: lista arquivos e diretórios\n");
    printf("  touch <file>: cria um arquivo vazio\n");
    printf("  cat <file>: exibe o conteúdo do arquivo\n");
    printf("  echo <content> > <file>: escreve conteúdo em um arquivo\n");
    printf("  cp <src> <dest>: copia arquivos\n");
    printf("  mv <src> <dest>: move ou renomeia arquivos\n");
    printf("  rm <name>: remove arquivo ou diretório\n");
    printf("  chmod <perm> <file>: altera permissões\n");
    printf("  stat <file>: exibe metadados\n");
    printf("  exit: encerra o programa\n");
}
