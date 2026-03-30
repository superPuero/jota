#include "hash.h"

jo_hash jo_fnv1_hash_str(jo_str_view str_view)	
{
	jo_u64 hash = jo_fnv1_offset_bias;

	for (jo_i32 i = 0; i < str_view.len; ++i) 
	{
		hash ^= (jo_u64)str_view.data[i];
		hash *= jo_fnv1_prime;
	}

	return (jo_hash)hash;
}
