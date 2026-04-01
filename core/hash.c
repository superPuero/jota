#include "hash.h"

u64 fnv1_hash_str(strv strv)	
{
	u64 out = fnv1_offset_bias;

	for (i32 i = 0; i < strv.len; ++i) 
	{
		out ^= (u64)strv.data[i];
		out *= fnv1_prime;
	}

	return (u64)out;
}
