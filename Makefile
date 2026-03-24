SRCS = main.c \
       core/string.c \
       compiler/lexer.c \
       compiler/token.c \
       compiler/parser.c \
       compiler/ast_node.c \
       compiler/symbol.c \
       compiler/sema.c \
       compiler/bytecode.c \
       compiler/vm.c \
       compiler/codegen.c

CC = clang # clang for pdbs

SRCS = main.c \
       core/string.c \
       compiler/lexer.c \
       compiler/token.c \
       compiler/parser.c \
       compiler/ast_node.c \
       compiler/symbol.c \
       compiler/sema.c \
       compiler/bytecode.c \
       compiler/vm.c \
       compiler/codegen.c

CC = gcc

compile:
	$(CC) -g -O0 -std=c99 $(SRCS) -o jota.exe