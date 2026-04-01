#ifndef jota_arena
#define jota_arena

#include <stdlib.h>
#include "types.h"
#include "utils.h"

#define Kb(expr) (expr) * 1024ULL
#define Mb(expr) Kb(expr) * 1024ULL
#define Gb(expr) Mb(expr) * 1024ULL

#define arena_name_max_len 256

typedef struct
{
	u8* mem;
	uz current_marker;
	uz current;
	uz capacity;
	char name[arena_name_max_len];
} arena;

typedef struct
{
	arena* arena;
	uz point;
} marker;

#define arena_ppush(arena, type) arena_push(arena, sizeof(type), alignof(type), true)

#define arena_scope(arena)\
for(i32 _i = (marker(arena), 0); _i < 1; arena_pop_to_marker(arena), ++_i)

arena arena_make(uz capacity, strv name);

void arena_release(arena* arena);
marker arena_mark(arena* arena); 
void arena_pop_to_marker(marker); 

void* arena_push(arena* arena, uz size, uz alignment, bool8 zero);

#endif