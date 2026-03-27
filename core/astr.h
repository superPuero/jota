#ifndef jota_astr
#define jota_astr

#include "types.h"
#include "arena.h"

#define jo_astr_initial_capacity 32
#define jo_astr_growth_coef 2

typedef struct
{
	char* data;
	jo_uz size;
	jo_uz capacity;
} jo_astr_t;

#endif

