#ifndef jota_hash
#define jota_hash

#include "types.h"
#include "utils.h"

#define jo_fnv1_offset_bias 14695981039346656037ull
#define jo_fnv1_prime 1099511628211ull

typedef jo_u64 jo_hash; 

jo_hash jo_fnv1_hash_str(jo_str_view str_view);

#endif