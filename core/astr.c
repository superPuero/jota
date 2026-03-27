#include "astr.h"

jo_astr_t jo_astr_from(jo_arena_t* arena, const char* str)
{
	jo_astr_t out = {0};
	jo_astr_append_view(arena, &out, jo_str_view_from_cstr(str));
	return out;
}


jo_astr_t jo_astr_from_view(jo_arena_t* arena, jo_str_view_t view)
{
	jo_astr_t out = {0};
	jo_astr_append_view(arena, &out, view);
	return out;
}

void jo_astr_append(jo_arena_t* arena, jo_astr_t* str, const char* cstr)
{
	jo_astr_append_view(arena, str, jo_str_view_from_cstr(cstr));
}

void jo_astr_append_astr(jo_arena_t* arena, jo_astr_t* str, jo_astr_t* astr)
{
	jo_astr_append_view(arena, str, jo_str_view_from(astr->data, astr->occupied));
}

void jo_astr_append_view(jo_arena_t* arena, jo_astr_t* str, jo_str_view_t view)
{
	for(jo_i32 i = 0; i < view.len; i++)
	{
		jo_ada_append(arena, str, view.data[i]);
	}
}