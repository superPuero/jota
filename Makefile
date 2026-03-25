CORE_SRC = 	core/string.c \
       		core/arena.c \

COMPILER_SRC = \
       compiler/compile_options.c \
       compiler/lexer.c \
       compiler/token.c \
       compiler/parser.c \
       compiler/ast_node.c \
       compiler/symbol.c \
       compiler/sema.c \
       compiler/bytecode.c \
       compiler/vm.c \

CC = gcc

compile:
	$(CC) -g -O0 -std=c99 main.c $(CORE_SRC) $(COMPILER_SRC) -o jota.exe	