#ifndef jota_dyn_array
#define jota_dyn_array

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "arena.h"

// @perf: tweakin these migh be benefitial 
#define ada_growth_coef 2
#define ada_initial_capacity 8

#define ada_declare(data_type, decl_type)\
typedef struct\
{\
	data_type* it;\
	data_type* data;\
	uz occupied;\
	uz capacity;\
} decl_type;

#define ada_append(arena, arr, ...)\
do{\
	if((arr)->occupied == (arr)->capacity)\
	{\
		uz elem_size = sizeof((arr)->data[0]);\
		(arr)->capacity = !(arr)->capacity ? ada_initial_capacity : (arr)->capacity * ada_growth_coef;\
		void* new_data = arena_push(arena, (arr)->capacity * elem_size, 8, true);\
		if((arr)->occupied) { memcpy(new_data, (arr)->data, elem_size * (arr)->occupied); }\
		(arr)->data = new_data;\
	}\
	(arr)->data[(arr)->occupied++] = __VA_ARGS__;\
}while(0)

#define ada_last(arr) ((arr)->data + (arr)->occupied - 1)

#define ada_foreach(ada)\
for(uz _i = ((ada)->it = NULL, 0); _i < (ada)->occupied && ((ada)->it = (ada)->data + _i, 1); ++_i)

#define ada_foreach_named(ada, _it)\
for(uz _it = ((ada)->it = NULL, 0); _it < (ada)->occupied && ((ada)->it = (ada)->data + _it, 1); ++_it)

#endif