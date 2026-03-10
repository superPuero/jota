#ifndef jota_utils
#define jota_utils

#include <stdlib.h>
#include <stdio.h>

#define jo_stringify_case(value)\
case value:\
return #value

#define jo_err(fmt, ...)\
printf(fmt"\n", __VA_ARGS__);\
exit(1)

#endif	