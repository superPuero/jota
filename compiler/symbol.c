#include "symbol.h"

jo_symbol_t jo_make_symbol(jo_arena_t* arena, jo_str_view_t identifier, jo_symbol_kind_t kind)
{
	jo_symbol_t symbol ={0};

	symbol.kind = kind;
	symbol.identifier = jo_astr_from_view(arena, identifier);

	return symbol;
}

jo_symbol_t* jo_scope_lookup_symbol(jo_scope_t* scope, jo_str_view_t identifier)
{
	if(!scope) return NULL;

	jo_ada_foreach(&scope->symbol_table)
	{
			if (strncmp(scope->symbol_table.it->identifier.data, identifier.data, identifier.len) == 0) 
			{
				return scope->symbol_table.it; 
			}
    }

	return jo_scope_lookup_symbol(scope->parent, identifier);
}

jo_scope_t* jo_scope_push(jo_arena_t* arena, jo_scope_t* scope, jo_str_view_t identifier)
{
	jo_scope_t* new_scope = jo_arena_palloc(arena, jo_scope_t);
	new_scope->parent = scope;
	new_scope->identifier = jo_astr_from_view(arena, identifier);
	return new_scope;
}

jo_symbol_t* jo_scope_add_symbol(jo_arena_t* arena, jo_scope_t* scope, jo_symbol_t symbol)
{
	jo_ada_append(arena, &scope->symbol_table, symbol);

	return &scope->symbol_table.data[scope->symbol_table.occupied - 1];
}