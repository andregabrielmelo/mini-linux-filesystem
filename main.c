#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/filesystem/filesystem.h"
#include "libs/user/user.h"

// Caminho do diretório atual
char* get_directory_fullpath(ext4_inode* cwd) {
    // Caminho simulado: apenas "/" porque não há árvore real com nomes
    char* path = malloc(16);
    strcpy(path, "/");
    return path;
}

int main() {

    // Simula um usuário atual (simplificado)
    user current_user = { .name = "root" };

    // Ponteiro para o diretorio atual (current working directory)
    ext4_inode* current_directory;

    // Inicializa raiz e define cwd
    current_directory = create_inode(EXT4_FT_DIR, 0755);
    dir_entries[1] = calloc(10, sizeof(ext4_dir_entry));
    inode_table[1] = current_directory;

    // cmd vai guadar a entrada do usurio
    char cmd[256];
    printf("MiniFS iniciado. Digite 'help' para comandos. Digite 'exit' para sair.\n");
    while (1) {
        // Mostra o sitema, usuario atual e nome do diretorio atual
        char* fullpath = get_directory_fullpath(current_directory);
        printf("[%s@MiniFS %s]$ ", current_user.name, fullpath);
        free(fullpath);

        // Pega a entrada do usuario quando ele da enter (new line)
        if (!fgets(cmd, sizeof(cmd), stdin)) continue;

         // Limpa a entrada do usuario, substituindo o new line '\n' pelo string terminator '\0
        cmd[strcspn(cmd, "\n")] = 0;

         // Chama os comandos de acordo com o que foi digitado
        if (strcmp(cmd, "exit") == 0) 
            break;
        else if (strcmp(cmd, "help") == 0) 
            help();
        else if (strncmp(cmd, "mkdir ", 6) == 0) {
            char *dirname =  cmd + 6;
            mkdir(current_directory, cmd + );
        }
        else if (strncmp(cmd, "cd ", 3) == 0) {
            char *dirname = cmd + 3;
            current_directory = cd(current_directory, dirname);
        }
        else if (strcmp(cmd, "ls") == 0) {
            ls(current_directory, NULL);
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
                chmod(current_directory, file, (mode_t)perm);
            } else {
                printf("Uso: chmod <perm> <arquivo>\n");
            }
        }
        else if (strncmp(cmd, "stat ", 5) == 0) {
            stat(current_directory, cmd + 5);
        }
        else {
            printf("%s: comando não encontrado\n", cmd);
        }
    }

    return 0;
}
