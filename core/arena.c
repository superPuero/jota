#include <assert.h>
#include <stdio.h>
#include "arena.h"

jo_arena jo_arena_make(jo_uz capacity, jo_str_view name)
{
	jo_arena arena = {.mem = malloc(capacity), .markers = {0}, .name = {0}, .current_marker = 0, .current = 0 , .capacity = capacity};
	memcpy(arena.name, name.data, name.len);
	return arena;
}

void jo_arena_free(jo_arena* arena)
{
	free(arena->mem);
}

void jo_arena_marker(jo_arena*arena)
{
	arena->markers[arena->current_marker++] = arena->current;
}

void jo_arena_pop_to_marker(jo_arena*arena)
{
	arena->current = arena->markers[--arena->current_marker];
}

void* jo_arena_alloc_aligned(jo_arena*arena, jo_uz size, jo_uz alignment)
{	
	jo_ptr unaligned_current = (jo_ptr)(arena->mem + arena->current);
	jo_u32 align_offset = (jo_u32)(((unaligned_current + (alignment - 1)) & ~(alignment - 1)) - unaligned_current);
	arena->current += align_offset;

	jo_u8* out = arena->mem + arena->current;
	arena->current += size;

	if(arena->current > arena->capacity)
	{
		printf("arena [%s] overflow\n", arena->name);
		assert(0);
	}

	return out;
}

void* jo_arena_alloc_aligned_zeroed(jo_arena*arena, jo_uz size, jo_uz alignment)
{		
	jo_u8* out = jo_arena_alloc_aligned(arena, size, alignment);
	memset(out, 0, size);
	return out;
}

void* jo_arena_alloc(jo_arena*arena, jo_uz size)
{	
	return jo_arena_alloc_aligned(arena, size, 1);
}

void* jo_arena_alloc_zeroed(jo_arena*arena, jo_uz size)
{	
	jo_u8* out = jo_arena_alloc(arena, size);
	memset(out, 0, size);
	return out;
}


