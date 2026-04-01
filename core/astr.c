#include "astr.h"


str_view str_view_from_astr(const astr* astr)
{
	return (str_view){.data = astr->data, .len = astr->occupied};
}

astr astr_from(arena* arena, const char* str)
{
	astr out = {0};
	astr_append_view(arena, &out, str_view_from(str));
	return out;
}

astr astr_from_nt(arena* arena, const char* str)
{
	astr out = astr_from(arena, str);
	astr_append(arena, &out, "\0");
	return out;
}

astr astr_from_view(arena* arena, str_view view)
{
	astr out = {0};
	astr_append_view(arena, &out, view);
	return out;
}

astr astr_from_view_nt(arena* arena, str_view view)
{
	astr out = astr_from_view(arena, view);
	astr_append(arena, &out, "\0");
	return out;
}

astr astr_clone(arena* arena, const astr* str)
{
	return astr_from_view(arena, str_view_from_astr(str));
}

void astr_append(arena* arena, astr* str, const char* cstr)
{
	astr_append_view(arena, str, str_view_from(cstr));
}

void astr_append_astr(arena* arena, astr* str, astr* astr)
{
	astr_append_view(arena, str, str_view_make(astr->data, astr->occupied));
}

void astr_append_view(arena* arena, astr* str, str_view view)
{
	for(i32 i = 0; i < view.len; i++)
	{
		ada_append(arena, str, view.data[i]);
	}
}