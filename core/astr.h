// non-null terminated arena based string
#ifndef jota_astr
#define jota_astr

#include "types.h"
#include "utils.h"
#include "ada.h"

ada_declare(char, astr);

#define astr_fmt(astr) (i32)(astr)->occupied, (astr)->data

str_view str_view_from_astr(const astr* astr);
astr astr_from(arena* arena, const char* str);
astr astr_from_nt(arena* arena, const char* str);
astr astr_from_view(arena* arena, str_view view);
astr astr_from_view_nt(arena* arena, str_view view);
astr astr_clone(arena* arena, const astr* str);
void astr_append(arena* arena, astr* str, const char* cstr);
void astr_append_astr(arena* arena, astr* str, astr* astr);
void astr_append_view(arena* arena, astr* str, str_view view);

#endif

