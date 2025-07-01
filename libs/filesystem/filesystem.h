// filesystem.h
#pragma once

#include <stdint.h>
#include <time.h>

#define EXT4_NAME_LEN 255
#define EXT4_N_BLOCKS 15
#define MAX_INODES 1024

// Enum para os tipos de arquivos suportados
typedef enum {
    EXT4_FT_UNKNOWN = 0,
    EXT4_FT_REG_FILE = 1,
    EXT4_FT_DIR = 2,
} ext4_file_type;

// Enum para permissões estilo Unix (RWX) com bitmasks
typedef enum {
    MODE_NONE   = 0,
    MODE_EXEC   = 1 << 0,
    MODE_WRITE  = 1 << 1,
    MODE_READ   = 1 << 2,
    MODE_OWNER_SHIFT = 6,
    MODE_GROUP_SHIFT = 3,
    MODE_OTHER_SHIFT = 0
} mode_t;

// Estrutura simulando um inode (FCB)
typedef struct {
    int32_t mode;                         // Permissões e tipo do arquivo
    int32_t uid;                          // UID do proprietário
    int32_t size;                         // Tamanho do arquivo (em bytes)
    time_t atime;                         // Último acesso
    time_t ctime;                         // Criação / modificação do inode
    time_t mtime;                         // Modificação dos dados
    time_t dtime;                         // Tempo de exclusão
    int32_t gid;                          // GID do grupo
    int32_t links_count;                  // Contador de links
    int32_t blocks[EXT4_N_BLOCKS];        // Blocos de dados (simulado)
} ext4_inode;

// Entrada de diretório
typedef struct {
    int32_t inode;                        // Número do inode
    int32_t size;                         // Tamanho da entrada
    int32_t name_len;                     // Comprimento do nome
    ext4_file_type file_type;             // Tipo de arquivo
    char name[EXT4_NAME_LEN];             // Nome do arquivo/diretório
} ext4_dir_entry;

// Interface de comandos do sistema de arquivos
void ls(ext4_inode* cwd, const char* filename);
void ls_l(ext4_inode* cwd, const char* filename);
void mkdir(ext4_inode* cwd, const char* dirname);
void touch(ext4_inode* cwd, const char* filename);
void echo(ext4_inode* cwd, const char* content, const char* dest_filename);
void cat(ext4_inode* cwd, const char* filename);
void cp(ext4_inode* cwd, const char* src, const char* dest);
void mv(ext4_inode* cwd, const char* src, const char* dest);
void rm(ext4_inode* cwd, const char* filename);
void chmod(ext4_inode* cwd, const char* filename, mode_t mode);
void stat(ext4_inode* cwd, const char* filename);
ext4_inode* cd(ext4_inode* cwd, const char* dirname) 

// Utilitários
int is_directory(ext4_inode* inode);
int is_regular_file(ext4_inode* inode);
int32_t allocate_inode()
ext4_inode* lookup(ext4_inode* cwd, const char* filename);
void check_permissions(ext4_inode* inode, mode_t required);
void print_permissions(int32_t mode);
void help();