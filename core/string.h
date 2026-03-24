#ifndef jota_string
#define jota_string

#include <stdlib.h>
#include <string.h>
#include "types.h"

#define jo_string_growth_coef 2

typedef struct
{
	jo_u8* data;
	jo_usize size;
	jo_usize capacity;
} jo_string;

bool jo_str_ends_with(const char* str, const char* suffix);
jo_string jo_string_from(const char* chars);
jo_string jo_string_from_n(const char* chars, jo_u32 len);
void jo_string_append_ch(jo_string* str, char ch);
void jo_string_append(jo_string* str, const char* chars);
void jo_string_free(jo_string* string);

#endif