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
	f64 time;
} profile_entry;

ada_declare(profile_entry, profiler);

#define profile(arena, profiler, pname)\
clock_t pname##begin = clock();\
clock_t pname##end = 0.0;\
ada_append(arena, profiler, (profile_entry){.name = #pname});\
profile_entry* pname##entry = ada_last(profiler);\
for(u32 pname##i = 0; pname##i < 1; pname##end = clock(), pname##entry->time = ((f64)(pname##end - pname##begin) / CLOCKS_PER_SEC), ++pname##i)

#endif


