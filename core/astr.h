#ifndef jota_astr
#define jota_astr

#include "types.h"
#include "utils.h"
#include "ada.h"

jo_ada_declare(char, jo_astr);

#define jo_astr_fmt(astr) (jo_i32)(astr)->occupied, (astr)->data

jo_str_view jo_str_view_from_astr(const jo_astr* astr);
jo_astr jo_astr_from(jo_arena* arena, const char* str);
jo_astr jo_astr_from_nt(jo_arena* arena, const char* str);
jo_astr jo_astr_from_view(jo_arena* arena, jo_str_view view);
jo_astr jo_astr_from_view_nt(jo_arena* arena, jo_str_view view);
jo_astr jo_astr_clone(jo_arena* arena, const jo_astr* str);
void jo_astr_append(jo_arena* arena, jo_astr* str, const char* cstr);
void jo_astr_append_astr(jo_arena* arena, jo_astr* str, jo_astr* astr);
void jo_astr_append_view(jo_arena* arena, jo_astr* str, jo_str_view view);

#endif

