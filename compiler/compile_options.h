#ifndef jota_compile_options
#define jota_compile_options

#include "../core/core.h"

#define compile_options_list\
	XY(h, "compiler flags help")\
	XY(t, "dump tokens into stdout")\
	XY(ast, "dump ast into stdout")\
	XY(bc, "dump bytecode into stdout")\
	XY(i, "run/interperet the bytecode")\
	XY(time, "output compiler timings report")\
	XY(mem, "output compiler memory usage report")

typedef struct
{
	#define XY(e, help_info) bool8 e;
	compile_options_list
	#undef XY

	bool8 success;
	bool8 file_provided;
	strv file;

} compile_options;

compile_options compile_options_parse_from_args(int argc, char** argv);

#endif
