#ifndef jota_profile
#define jota_profile

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include "types.h"
#include "ada.h"

typedef struct
{
	const char* name;
	jo_f64 time;
} jo_profile_entry;

jo_ada_declare(jo_profile_entry, jo_profiler);

#define jo_profile(arena, profiler, pname)\
clock_t pname##begin = clock();\
clock_t pname##end = 0.0;\
jo_ada_append(arena, profiler, (jo_profile_entry){.name = #pname});\
jo_profile_entry* pname##entry = jo_ada_last(profiler);\
for(jo_u32 pname##i = 0; pname##i < 1; pname##end = clock(), pname##entry->time = ((jo_f64)(pname##end - pname##begin) / CLOCKS_PER_SEC), ++pname##i)

#endif


