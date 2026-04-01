#ifndef jota_file
#define jota_file

#include "utils.h"
#include "str.h"

typedef struct
{
	str name;
	char* data;
	uz 	len;
}file;

file* file_load(arena* arena, strv name);

#endif