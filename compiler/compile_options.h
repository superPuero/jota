#ifndef jota_compile_options
#define jota_compile_options

#include "../core/core.h"

#define jo_compile_options_list\
	X(t)\
	X(ast)\
	X(sema)\
	X(bc)\
	X(i)\
	X(time)\
	X(mem)

#define jo_compile_ast_propagate_list\
	X(t)\

#define jo_compile_sema_propagate_list\
	jo_compile_ast_propagate_list\
	X(ast)

#define jo_compile_bc_propagate_list\
	jo_compile_sema_propagate_list\
	X(sema)

#define jo_compile_i_propagate_list\
	jo_compile_bc_propagate_list\
	X(bc)


typedef struct
{
	#define X(e) jo_bool e;
	jo_compile_options_list
	#undef X
	jo_bool success;
	jo_str_view filepath;
}jo_compile_options;

jo_compile_options jo_compile_options_parse_from_args(int argc, char** argv);


#endif

