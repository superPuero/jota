#include "arena.h"

jo_arena_t jo_arena_make(uint32_t capacity)
{
	return (jo_arena_t){.mem = malloc(capacity), .markers = 0, .current_marker = 0, .current = 0 , .capacity = capacity};
}


void jo_arena_free(jo_arena_t* arena)
{
	free(arena->mem);
}

void jo_arena_marker(jo_arena_t *arena)
{
	arena->markers[arena->current_marker++] = arena->current;
}

void jo_arena_pop_to_marker(jo_arena_t *arena)
{
	arena->current = arena->markers[--arena->current_marker];
}

void* jo_arena_push(jo_arena_t *arena, uint32_t size)
{	
	char* curr = arena->mem + arena->current;
	arena->current += size;
	return curr;
}