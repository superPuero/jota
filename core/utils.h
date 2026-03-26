#ifndef jota_utils
#define jota_utils

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#define jo_alignof(type) offsetof(struct { char c; type d; }, d)

#define jo_stringify_case(value)\
case value:\
return #value

#endif	