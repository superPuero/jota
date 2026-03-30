#ifndef jota_file
#define jota_file

#include "utils.h"
#include "astr.h"

typedef struct
{
	jo_astr name;
	char* data;
	jo_uz 	len;
}jo_file;

jo_file* jo_file_load(jo_arena* arena, const jo_astr* name);

#endif