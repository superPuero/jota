#ifndef jota_file
#define jota_file

#include "utils.h"
#include "astr.h"

typedef struct
{
	astr name;
	char* data;
	uz 	len;
}file;

file* file_load(arena* arena, str_view name);

#endif