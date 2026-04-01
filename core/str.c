#include "str.h"


strv strv_from_str(const str* str)
{
	return (strv){.data = str->data, .len = str->occupied};
}

str str_from_cstr(arena* arena, const char* cstr)
{
	str out = {0};
	str_append_view(arena, &out, strv_from_cstr(cstr));
	return out;
}

str str_from_cstr_nt(arena* arena, const char* cstr)
{
	str out = str_from_cstr(arena, cstr);
	str_append_char(arena, &out, '\0');
	return out;
}

str str_from_view(arena* arena, strv view)
{
	str out = {0};
	str_append_view(arena, &out, view);
	return out;
}

str str_from_view_nt(arena* arena, strv view)
{
	str out = str_from_view(arena, view);
	str_append_char(arena, &out, '\0');
	return out;
}

str str_clone(arena* arena, const str* clonee)
{
	return str_from_view(arena, strv_from_str(clonee));
}

void str_append_char(arena* arena, str* str, char ch)
{
	da_append(arena, str, ch);
}

void str_append_cstr(arena* arena, str* str, const char* cstr)
{
	str_append_view(arena, str, strv_from_cstr(cstr));
}

void str_append_str(arena* arena, str* to, str* what)
{
	str_append_view(arena, to, strv_from_str(what));
}

void str_append_view(arena* arena, str* str, strv view)
{
	// @memory: this might kill memory at some point
	for(i32 i = 0; i < view.len; i++)
	{
		da_append(arena, str, view.data[i]);
	}
}