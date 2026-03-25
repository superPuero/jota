#ifndef jota_compile_options
#define jota_compile_options

#include <stdbool.h>
#include "../core/core.h"

typedef struct
{
	jo_bool tokens;
	jo_bool ast;
	jo_bool sema;
	jo_bool bytecode;
	jo_bool interp;
	jo_bool success;
}jo_compile_options;

jo_compile_options jo_compie_options_parse_from_args(int argc, char** argv);


#endif

