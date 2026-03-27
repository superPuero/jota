#include "symbol.h"

jo_symbol jo_make_symbol(jo_arena* arena, jo_str_view identifier, jo_symbol_kind kind)
{
	jo_symbol symbol ={0};

	symbol.kind = kind;
	symbol.identifier = jo_astr_from_view(arena, identifier);

	return symbol;
}

jo_symbol* jo_scope_lookup_symbol(jo_scope* scope, jo_str_view identifier)
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

jo_scope* jo_scope_push(jo_arena* arena, jo_scope* scope, jo_str_view identifier)
{
	jo_scope* new_scope = jo_arena_palloc(arena, jo_scope);
	new_scope->parent = scope;
	new_scope->identifier = jo_astr_from_view(arena, identifier);
	return new_scope;
}

jo_symbol* jo_scope_add_symbol(jo_arena* arena, jo_scope* scope, jo_symbol symbol)
{
	jo_ada_append(arena, &scope->symbol_table, symbol);

	return &scope->symbol_table.data[scope->symbol_table.occupied - 1];
}