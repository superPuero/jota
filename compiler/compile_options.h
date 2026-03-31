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

