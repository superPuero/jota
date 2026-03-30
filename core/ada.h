#ifndef jota_dyn_array
#define jota_dyn_array

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "arena.h"

// @perf: tweakin these migh be benefitial 
#define jo_ada_growth_coef 2
#define jo_ada_initial_capacity 8

#define jo_ada_declare(data_type, decl_type)\
typedef struct\
{\
	data_type* it;\
	data_type* data;\
	jo_uz occupied;\
	jo_uz capacity;\
} decl_type;

#define jo_ada_append(arena, arr, ...)\
do{\
	if((arr)->occupied == (arr)->capacity)\
	{\
		jo_uz elem_size = sizeof((arr)->data[0]);\
		(arr)->capacity = !(arr)->capacity ? jo_ada_initial_capacity : (arr)->capacity * jo_ada_growth_coef;\
		void* new_data = jo_arena_alloc_aligned_zeroed(arena, (arr)->capacity * elem_size, 8);\
		if((arr)->occupied) { memcpy(new_data, (arr)->data, elem_size * (arr)->occupied); }\
		(arr)->data = new_data;\
	}\
	(arr)->data[(arr)->occupied++] = __VA_ARGS__;\
}while(0)

#define jo_ada_last(arr) ((arr)->data + (arr)->occupied - 1)

#define jo_ada_foreach(ada)\
for(jo_uz _i = ((ada)->it = NULL, 0); _i < (ada)->occupied && ((ada)->it = (ada)->data + _i, 1); ++_i)

#define jo_ada_foreach_named(ada, _it)\
for(jo_uz _it = ((ada)->it = NULL, 0); _it < (ada)->occupied && ((ada)->it = (ada)->data + _it, 1); ++_it)

#endif