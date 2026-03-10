#ifndef jota_dyn_array
#define jota_dyn_array

#include <stdlib.h>
#include <string.h>
#include "types.h"

#define jo_dyn_array_growth_coef 2
#define jo_dyn_array_initial_capacity 8

#define jo_decl_dyn_array(type)\
typedef struct\
{\
	type* data;\
	jo_usize occupied;\
	jo_usize capacity;\
} type##_dyn_array_t;

#define jo_decl_dyn_array_named(type, typename)\
typedef struct\
{\
	type* data;\
	jo_usize occupied;\
	jo_usize capacity;\
} typename;

#define jo_dyn_array_append(arr, ...)\
if((arr)->occupied == (arr)->capacity)\
{\
	(arr)->capacity = !(arr)->capacity ? jo_dyn_array_initial_capacity : (arr)->capacity * jo_dyn_array_growth_coef;\
	(arr)->data = realloc((arr)->data, sizeof((arr)->data[0]) * (arr)->capacity);\
}\
(arr)->data[(arr)->occupied++] = __VA_ARGS__;

#define jo_dyn_array_pop(arr)\
(arr)->occupied--;

#define jo_dyn_array_pop_by(arr, ammount)\
(arr)->occupied - ammount;

#define jo_dyn_array_free(arr)\
free((arr)->data);\
memset(arr, 0, sizeof(*arr))\

#define jo_dyn_array_iter(arr, itername, ...)\
{\
	for(jo_usize itername = 0; itername < (arr)->occupied; itername++)\
	__VA_ARGS__\
}


#endif