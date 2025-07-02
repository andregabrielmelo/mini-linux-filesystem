# MiniFS – Simulador de Sistema de Arquivos Linux-like

Este projeto é um simulador de sistema de arquivos construído em C, inspirado nos conceitos de sistemas Unix/Linux. Ele fornece uma interface de linha de comando interativa para navegar, criar, modificar e visualizar arquivos e diretórios simulados na memória.

Me inspirei bastante a forma como o ext4 foi implementado na [documentação do linux](https://www.kernel.org/doc/html/latest/filesystems/ext4)

## 🔧 Compilação

Para compilar o projeto, execute:

```bash
make
```

Requisitos:

- Compilador `gcc`
- Makefile incluso no projeto
- Estrutura de diretórios `libs/` corretamente mantida

## ▶️ Execução

Após a compilação:

```bash
./bin/main
```

Use `help` para ver os comandos suportados e `clear` para limpar a tela conforme necessário.

## 📂 Estruturas de Dados e Design

### `inode` e `file_control_block (FCB)`

- Cada arquivo ou diretório é representado por um `inode`.
- Contém metadados e ponteiro para o pai (`parent`), mais conteúdo (arquivo) ou entradas (diretório).

### Diretórios como Árvores

- Diretórios simulados com ponteiro para o pai.
- Reconstrução de caminho absoluto via `get_directory_fullpath`.

## 📚 Conceitos Implementados

- Atributos de arquivo (nome, tamanho, timestamps, permissões)
- Operações com arquivos (criação, leitura, escrita, exclusão)
- FCB como estrutura `file_control_block`
- Permissões RWX por proprietário, grupo e outros
- Estrutura de diretórios em árvore
- Simulação de alocação de blocos com `char content[]`

## 💻 Exemplo

```bash
mkdir projetos
cd projetos
touch readme.txt
echo exemplo > readme.txt
cat readme.txt
chmod 600 readme.txt
stat readme.txt
ls
```

## 📁 Organização

```
.
├── main.c
├── Makefile
├── README.md
├── libs/
│   ├── fcb/
│   ├── filesystem/
│   ├── user/
│   └── permission/
└── bin/
```

## 👨‍🏫 Objetivo

Trabalho para a disciplina de Sistemas Operacionais, abordando:

- Organização e simulação de sistemas de arquivos
- Conceitos práticos de estrutura de diretórios e permissões
