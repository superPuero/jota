#include "file.h"

file* file_load(arena* arena, strv filename)
{
	FILE* cfile = NULL;

	marker marker = arena_mark(arena);

	str filename_nt = str_from_view_nt(arena, filename);		
	cfile = fopen(filename_nt.data, "rb");	

	arena_pop_to_marker(marker);

	if (!cfile)
	{
		printf("error: could not open file '%.*s'\n", strv_fmt(&filename));
		return NULL; 
	}

	file* out = arena_ppush(arena, file);
	fseek(cfile, 0, SEEK_END);
	out->len = ftell(cfile);
	rewind(cfile);

	out->data = arena_push(arena, out->len, 1, false);

	fread(out->data, 1, out->len, cfile);
	
	fclose(cfile);

	return out;
}