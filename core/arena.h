#ifndef jota_areana
#define jota_areana

#include <stdlib.h>
#include "types.h"

#define jo_Kb(expr) (expr) * 1024
#define jo_Mb(expr) jo_Kb(expr) * 1024
#define jo_Gb(expr) jo_Mb(expr) * 1024

#define jo_arena_max_marker_depth 128	

typedef struct
{
	jo_u8* mem;
	jo_u32 markers[jo_arena_max_marker_depth];
	jo_u32 current_marker;
	jo_u32 current;
	jo_u32 capacity;
} jo_arena_t;

jo_arena_t jo_arena_make(uint32_t capacity);
void jo_arena_free(jo_arena_t* arena);
void jo_arena_marker(jo_arena_t *arena);
void jo_arena_pop_to_marker(jo_arena_t *arena);
void* jo_arena_push(jo_arena_t *arena, uint32_t size);

#endif