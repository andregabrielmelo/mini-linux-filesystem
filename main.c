#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "libs/filesystem/filesystem.h"
#include "libs/user/user.h"

extern User current_user;

// Function to reverse a string in-place
void reverse_str(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; ++i) {
        char tmp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = tmp;
    }
}

// Get full path from cwd to root
char* get_directory_fullpath(inode* cwd) {
    char* path_parts[128]; // Array of pointers to directory names
    int count = 0;

    // Traverse upwards collecting names
    while (cwd->parent != NULL) {
        path_parts[count++] = cwd->fcb.name;
        cwd = cwd->parent;
    }

    // Allocate result buffer
    char* fullpath = malloc(1024);
    if (!fullpath) return NULL;
    fullpath[0] = '\0';

    // Rebuild path from root
    for (int i = count - 1; i >= 0; --i) {
        strcat(fullpath, "/");
        strcat(fullpath, path_parts[i]);
    }

    // Handle root case
    if (count == 0) {
        strcpy(fullpath, "/");
    }

    return fullpath;
}

int main() {
    // Define o usuário atual
    current_user = (User){ .username = "root", .id = 0, .group_id = 0 };

    // Cria a raiz do sistema de arquivos
    inode* root = malloc(sizeof(inode));
    memset(root, 0, sizeof(inode));
    strncpy(root->fcb.name, "/", NAME_MAX);
    root->fcb.file_type = DIRECTORY;
    root->fcb.data_type = CHARACTER;
    root->fcb.permissions = (Permission){7, 5, 5};
    root->parent = NULL;

    inode* current_directory = root;

    char cmd[256];
    printf("MiniFS iniciado. Digite 'help' para comandos. Digite 'exit' para sair.\n");
    while (1) {
        char* fullpath = get_directory_fullpath(current_directory);
        printf("[%s@MiniFS %s]$ ", current_user.username, fullpath);
        free(fullpath);

        if (!fgets(cmd, sizeof(cmd), stdin)) continue;
        cmd[strcspn(cmd, "\n")] = 0;

        if (strcmp(cmd, "exit") == 0) break;
        else if (strcmp(cmd, "clear") == 0) printf("\e[1;1H\e[2J");
        else if (strcmp(cmd, "help") == 0) help();
        else if (strncmp(cmd, "mkdir ", 6) == 0) {
            mkdir(current_directory, cmd + 6);
        }
        else if (strncmp(cmd, "cd ", 3) == 0) {
            inode* next = cd(current_directory, cmd + 3);
            if (next != NULL) current_directory = next;
        }
        else if (strcmp(cmd, "ls") == 0) {
            ls(current_directory, NULL);
        }
        else if (strncmp(cmd, "ls ", 3) == 0) {
            ls(current_directory, cmd + 3);
        }
        else if (strcmp(cmd, "ls -l") == 0) {
            ls_l(current_directory, NULL);
        }
        else if (strncmp(cmd, "touch ", 6) == 0) {
            touch(current_directory, cmd + 6);
        }
        else if (strncmp(cmd, "cat ", 4) == 0) {
            cat(current_directory, cmd + 4);
        }
        else if (strncmp(cmd, "echo ", 5) == 0) {
            char* p = strstr(cmd + 5, " > ");
            if (p) {
                *p = '\0';
                char* content = cmd + 5;
                char* filename = p + 3;
                echo(current_directory, content, filename);
            } else {
                printf("Uso: echo conteudo > arquivo\n");
            }
        }
        else if (strncmp(cmd, "cp ", 3) == 0) {
            char* src = strtok(cmd + 3, " ");
            char* dest = strtok(NULL, " ");
            if (src && dest) cp(current_directory, src, dest);
            else printf("Uso: cp <src> <dest>\n");
        }
        else if (strncmp(cmd, "mv ", 3) == 0) {
            char* src = strtok(cmd + 3, " ");
            char* dest = strtok(NULL, " ");
            if (src && dest) mv(current_directory, src, dest);
            else printf("Uso: mv <src> <dest>\n");
        }
        else if (strncmp(cmd, "rm ", 3) == 0) {
            rm(current_directory, cmd + 3);
        }
        else if (strncmp(cmd, "chmod ", 6) == 0) {
            char* perm_str = strtok(cmd + 6, " ");
            char* file = strtok(NULL, " ");
            if (perm_str && file) {
                int perm = atoi(perm_str);
                chmod_cmd(current_directory, file, perm);
            } else {
                printf("Uso: chmod <perm> <arquivo>\n");
            }
        }
        else if (strncmp(cmd, "stat ", 5) == 0) {
            stat_cmd(current_directory, cmd + 5);
        }
        else {
            printf("%s: comando não encontrado\n", cmd);
        }
    }

    return 0;
}
