#ifndef jota_compile_options
#define jota_compile_options

#include "../core/core.h"

#define jo_compile_options_list\
	XY(h, "compiler flags help")\
	XY(t, "dump tokens into stdout")\
	XY(ast, "dump ast into stdout")\
	XY(bc, "dump bytecode into stdout")\
	XY(i, "run/interperet the bytecode")\
	XY(time, "output compiler timings report")\
	XY(mem, "output compiler memory usage report")

typedef struct
{
	#define XY(e, help_info) jo_bool e;
	jo_compile_options_list
	#undef XY

	jo_bool success;
	jo_bool file_provided;
	jo_str_view file;

} jo_compile_options;

jo_compile_options jo_compile_options_parse_from_args(int argc, char** argv);

#endif
