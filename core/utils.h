#ifndef jota_utils
#define jota_utils

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include "types.h"

#define jo_alignof(type) offsetof(struct { char c; type d; }, d)

#define jo_stringify_case(value)\
case value:\
return #value

typedef struct
{
	const char* data;
	jo_i32 len;
} jo_str_view;


jo_str_view jo_str_view_make(const char* data, jo_uz len);
jo_str_view jo_str_view_from(const char* data);

#define jo_str_view_fmt(view) (view)->len, (view)->data



#endif	