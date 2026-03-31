CC = clang

COMMON_CC_OPT = -Wall -Wextra -std=c99 

WIN_DEBUG_CC_OPT = -O0 -g -gcodeview -fuse-ld=lld -Wl,--pdb=

RELEASE_CC_OPT = -O3 -march=native

CORE_SRC = 	core/astr.c \
       		core/arena.c \
       		core/utils.c \
       		core/file.c \
       		core/hash.c \


COMPILER_SRC = \
       compiler/compile_options.c \
       compiler/workspace.c \
       compiler/lexer.c \
       compiler/token.c \
       compiler/parser.c \
       compiler/ast_node.c \
       compiler/symbol.c \
       compiler/sema.c \
       compiler/bytecode.c \
       compiler/vm.c \


	
win_debug:
	$(CC) $(COMMON_CC_OPT) $(WIN_DEBUG_CC_OPT) main.c $(CORE_SRC) $(COMPILER_SRC) -o jota.exe	

release:
	$(CC) $(COMMON_CC_OPT) $(RELEASE_CC_OPT) main.c $(CORE_SRC) $(COMPILER_SRC) -o jota.exe	