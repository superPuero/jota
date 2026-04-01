// non-null terminated arena based string
#ifndef jota_str
#define jota_str

#include "types.h"
#include "utils.h"
#include "da.h"

da_declare(char, str);

#define str_fmt(str) (i32)(str)->occupied, (str)->data

strv strv_from_str(const str* str);

str str_from_cstr(arena* arena, const char* cstr);
str str_from_cstr_nt(arena* arena, const char* cstr);

str str_from_view(arena* arena, strv view);
str str_from_view_nt(arena* arena, strv view);

str str_clone(arena* arena, const str* clonee);

void str_append_char(arena* arena, str* str, char ch);
void str_append_cstr(arena* arena, str* str, const char* cstr);
void str_append_str(arena* arena, str* to, str* what);
void str_append_view(arena* arena, str* str, strv view);

#endif

