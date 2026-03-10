#include "symbol.h"

jo_symbol_t* jo_scope_lookup_symbol(jo_scope_t* scope, const char* identifier)
{
	if(!scope) return NULL;

	jo_dyn_array_iter(&scope->symbol_table, it, {
			if (strcmp(scope->symbol_table.data[it].identifier, identifier) == 0) 
			{
				return &scope->symbol_table.data[it]; 
			}
    	}
	);

	return jo_scope_lookup_symbol(scope->parent, identifier);
}

jo_scope_t* jo_scope_push(jo_scope_t* scope)
{
	jo_scope_t* new_scope = calloc(sizeof(jo_scope_t), 1);
	new_scope->parent = scope;
	return new_scope;
}

jo_scope_t* jo_scope_pop(jo_scope_t* scope)
{
	jo_scope_t* old_scope = scope->parent;	
	free(scope);
	return old_scope;
}

jo_symbol_t* jo_scope_add_symbol(jo_scope_t* scope, jo_symbol_t symbol)
{
	jo_dyn_array_append(&scope->symbol_table, symbol);
}