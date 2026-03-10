SRCS = main.c \
       compiler/parser.c \
       compiler/lexer.c \
       compiler/token.c \
       compiler/ast_node.c \
       compiler/symbol.c \
       compiler/sema.c \
       compiler/codegen.c

main:
	gcc -std=c99 $(SRCS) -o jota.exe