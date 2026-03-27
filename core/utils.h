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
} jo_str_view_t;

jo_str_view_t jo_str_view_from(const char* data, jo_uz len);

#define jo_profile(name)\
clock_t begin = clock();\
clock_t end = 0.0;\
for(jo_u32 i = 0; i < 1; end = clock(), printf("%s: %.0fms\n", name, ((jo_f64)(end - begin)) / CLOCKS_PER_SEC * 1000.0), ++i)

#endif	