#include "file.h"

jo_file* jo_file_load(jo_arena* arena, const jo_astr* filename)
{
	FILE* file = NULL;

	jo_arena_scope(arena)
	{
		jo_astr filename_nt = jo_astr_clone(arena, filename);		
		jo_astr_append(arena, &filename_nt, "\0"); // fopen only takes null terminated strings
		file = fopen(filename_nt.data, "rb");	
	}

	if (!file)
	{
		printf("error: could not open file '%.*s'\n", jo_astr_fmt(filename));
		return NULL; 
	}

	jo_file* out = jo_arena_palloc(arena, jo_file);
	fseek(file, 0, SEEK_END);
	out->len = ftell(file);
	rewind(file);

	out->data = jo_arena_alloc(arena, out->len);

	fread(out->data, 1, out->len, file);
	
	fclose(file);

	return out;
}