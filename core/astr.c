#include "astr.h"


jo_str_view jo_str_view_from_astr(const jo_astr* astr)
{
	return (jo_str_view){.data = astr->data, .len = astr->occupied};
}

jo_astr jo_astr_from(jo_arena* arena, const char* str)
{
	jo_astr out = {0};
	jo_astr_append_view(arena, &out, jo_str_view_from(str));
	return out;
}

jo_astr jo_astr_from_nt(jo_arena* arena, const char* str)
{
	jo_astr out = jo_astr_from(arena, str);
	jo_astr_append(arena, &out, "\0");
	return out;
}

jo_astr jo_astr_from_view(jo_arena* arena, jo_str_view view)
{
	jo_astr out = {0};
	jo_astr_append_view(arena, &out, view);
	return out;
}

jo_astr jo_astr_from_view_nt(jo_arena* arena, jo_str_view view)
{
	jo_astr out = jo_astr_from_view(arena, view);
	jo_astr_append(arena, &out, "\0");
	return out;
}

jo_astr jo_astr_clone(jo_arena* arena, const jo_astr* str)
{
	return jo_astr_from_view(arena, jo_str_view_from_astr(str));
}

void jo_astr_append(jo_arena* arena, jo_astr* str, const char* cstr)
{
	jo_astr_append_view(arena, str, jo_str_view_from(cstr));
}

void jo_astr_append_astr(jo_arena* arena, jo_astr* str, jo_astr* astr)
{
	jo_astr_append_view(arena, str, jo_str_view_make(astr->data, astr->occupied));
}

void jo_astr_append_view(jo_arena* arena, jo_astr* str, jo_str_view view)
{
	for(jo_i32 i = 0; i < view.len; i++)
	{
		jo_ada_append(arena, str, view.data[i]);
	}
}