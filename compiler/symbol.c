#include "symbol.h"

symbol make_symbol(arena* arena, str_view identifier, symbol_kind kind)
{
	symbol symbol ={0};

	symbol.kind = kind;
	symbol.identifier = astr_from_view(arena, identifier);

	return symbol;
}

symbol* scope_lookup_symbol(scope* scope, str_view identifier)
{
	if(!scope) return NULL;

	ada_foreach(&scope->symbol_table)
	{
		if (strncmp(scope->symbol_table.it->identifier.data, identifier.data, scope->symbol_table.it->identifier.occupied) == 0) 
		{
			return scope->symbol_table.it; 
		}
    }

	return scope_lookup_symbol(scope->parent, identifier);
}

scope* scope_push(arena* arena, scope* outer_scope, str_view identifier)
{
	scope* new_scope = arena_ppush(arena, scope);
	new_scope->parent = outer_scope;
	new_scope->identifier = astr_from_view(arena, identifier);
	return new_scope;
}

symbol* scope_add_symbol(arena* arena, scope* outer_scope, symbol symbol)
{
	ada_append(arena, &outer_scope->symbol_table, symbol);

	return &outer_scope->symbol_table.data[outer_scope->symbol_table.occupied - 1];
}