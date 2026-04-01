#ifndef jota_hash
#define jota_hash

#include "types.h"
#include "utils.h"

#define fnv1_offset_bias 14695981039346656037ull
#define fnv1_prime 1099511628211ull

u64 fnv1_hash_str(strv strv);

#endif