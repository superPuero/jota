#ifndef jota_astr
#define jota_astr

#include "types.h"
#include "utils.h"
#include "ada.h"

jo_ada_declare(char, jo_astr_t);

#define jo_astr_fmt(astr) (jo_i32)(astr)->occupied, (astr)->data

jo_astr_t jo_astr_from(jo_arena_t* arena, const char* str);
jo_astr_t jo_astr_from_view(jo_arena_t* arena, jo_str_view_t view);
void jo_astr_append(jo_arena_t* arena, jo_astr_t* str, const char* cstr);
void jo_astr_append_astr(jo_arena_t* arena, jo_astr_t* str, jo_astr_t* astr);
void jo_astr_append_view(jo_arena_t* arena, jo_astr_t* str, jo_str_view_t view);

#endif

