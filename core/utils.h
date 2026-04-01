#ifndef jota_utils
#define jota_utils

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include "types.h"

#define alignof(type) offsetof(struct { char c; type d; }, d)

#define expand(x) x 

#define stringify(x) #x

#define stringify_case(value)\
case value:\
return #value

#define stringify_expand_case(x)\
case expand(x):\
return stringify(expand(x))

typedef struct
{
	const char* data;
	i32 len;
} strv;

strv strv_make(const char* data, uz len);
strv strv_from_cstr(const char* data);

#define strv_fmt(view) (view)->len, (view)->data



#endif	