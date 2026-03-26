#ifndef jota_areana
#define jota_areana

#include <stdlib.h>
#include "types.h"

#define jo_Kb(expr) (expr) * 1024ULL
#define jo_Mb(expr) jo_Kb(expr) * 1024ULL
#define jo_Gb(expr) jo_Mb(expr) * 1024ULL

#define jo_arena_max_marker_depth 128	
#define jo_arena_name_max_len 256

typedef struct
{
	jo_u8* mem;
	jo_u32 markers[jo_arena_max_marker_depth];
	jo_u32 current_marker;
	jo_u32 current;
	jo_u32 capacity;
	char name[jo_arena_name_max_len];
} jo_arena_t;


#define jo_ada_growth_coef 2
#define jo_ada_initial_capacity 128

#define jo_declare_ada(data_type, decl_type)\
typedef struct\
{\
	data_type* data;\
	jo_uz occupied;\
	jo_uz capacity;\
} decl_type;

#define jo_ada_append(arena, arr, ...)\
if((arr)->occupied == (arr)->capacity)\
{\
	jo_uz elem_size = sizeof((arr)->data[0]);\
	(arr)->capacity = !(arr)->capacity ? jo_ada_initial_capacity : (arr)->capacity * jo_ada_growth_coef;\
	void* new_data = jo_arena_alloc_aligned_zeroed(arena, (arr)->capacity * elem_size, 8);\
	memcpy(new_data, (arr)->data, elem_size * (arr)->occupied);\
	(arr)->data = new_data;\
}\
(arr)->data[(arr)->occupied++] = __VA_ARGS__;

#define jo_arena_palloc(arena, type) jo_arena_alloc_aligned_zeroed(arena, sizeof(type), 8)

jo_arena_t jo_arena_make(jo_uz capacity, const char* name);
void jo_arena_free(jo_arena_t* arena);
void jo_arena_marker(jo_arena_t *arena);
void jo_arena_pop_to_marker(jo_arena_t *arena);
void* jo_arena_alloc(jo_arena_t *arena, jo_uz size);
void* jo_arena_alloc_aligned(jo_arena_t *arena, jo_uz size, jo_uz alignment);
void* jo_arena_alloc_aligned_zeroed(jo_arena_t *arena, jo_uz size, jo_uz alignment);
void* jo_arena_alloc(jo_arena_t *arena, jo_uz size);
void* jo_arena_alloc_zeroed(jo_arena_t *arena, jo_uz size);

#endif