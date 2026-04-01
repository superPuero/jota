#include "hash.h"

u64 fnv1_hash_str(str_view str_view)	
{
	u64 out = fnv1_offset_bias;

	for (i32 i = 0; i < str_view.len; ++i) 
	{
		out ^= (u64)str_view.data[i];
		out *= fnv1_prime;
	}

	return (u64)out;
}
