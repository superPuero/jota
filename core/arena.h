#ifndef jota_arena
#define jota_arena

#include <stdlib.h>
#include "types.h"
#include "utils.h"

#define jo_Kb(expr) (expr) * 1024ULL
#define jo_Mb(expr) jo_Kb(expr) * 1024ULL
#define jo_Gb(expr) jo_Mb(expr) * 1024ULL

#define jo_arena_max_marker_depth 128	
#define jo_arena_name_max_len 256

typedef struct
{
	jo_u8* mem;
	jo_uz markers[jo_arena_max_marker_depth];
	jo_uz current_marker;
	jo_uz current;
	jo_uz capacity;
	char name[jo_arena_name_max_len];
} jo_arena;

#define jo_arena_palloc(arena, type) jo_arena_alloc_aligned_zeroed(arena, sizeof(type), jo_alignof(type))

#define jo_arena_scope(arena)\
for(jo_i32 _i = (jo_arena_marker(arena), 0); _i < 1; jo_arena_pop_to_marker(arena), ++_i)

jo_arena jo_arena_make(jo_uz capacity, jo_str_view name);

void jo_arena_free(jo_arena* arena);
void jo_arena_marker(jo_arena* arena);
void jo_arena_pop_to_marker(jo_arena* arena);

void* jo_arena_alloc(jo_arena* arena, jo_uz size);
void* jo_arena_alloc_aligned(jo_arena* arena, jo_uz size, jo_uz alignment);
void* jo_arena_alloc_aligned_zeroed(jo_arena* arena, jo_uz size, jo_uz alignment);
void* jo_arena_alloc(jo_arena*arena, jo_uz size);
void* jo_arena_alloc_zeroed(jo_arena* arena, jo_uz size);

#endif