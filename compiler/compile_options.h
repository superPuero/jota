#ifndef jota_compile_options
#define jota_compile_options

#include <stdbool.h>
#include "../core/core.h"

typedef struct
{
	jo_bool tokens;
	jo_bool tokens_dump;
	jo_bool ast;
	jo_bool ast_dump;
	jo_bool sema;
	jo_bool bytecode;
	jo_bool bytecode_dump;
	jo_bool interp;
	jo_bool success;
	jo_bool time;
	jo_str_view filepath;
}jo_compile_options;

jo_compile_options jo_compile_options_parse_from_args(int argc, char** argv);


#endif

