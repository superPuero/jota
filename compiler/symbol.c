#include "symbol.h"

jo_symbol_t jo_make_symbol(const char* identifier, jo_symbol_kind_t kind)
{
	jo_symbol_t symbol ={0};

	symbol.kind = kind;
	memcpy(symbol.identifier.data, identifier, strlen(identifier));

	return symbol;
}

jo_symbol_t* jo_scope_lookup_symbol(jo_scope_t* scope, const char* identifier)
{
	if(!scope) return NULL;

	jo_dyn_array_iter(&scope->symbol_table, it, {
			if (strcmp(scope->symbol_table.data[it].identifier.data, identifier) == 0) 
			{
				return &scope->symbol_table.data[it]; 
			}
    	}
	);

	return jo_scope_lookup_symbol(scope->parent, identifier);
}

jo_scope_t* jo_scope_push(jo_scope_t* scope, const char* identifier)
{
	jo_scope_t* new_scope = calloc(sizeof(jo_scope_t), 1);
	new_scope->parent = scope;
	new_scope->identifier = jo_string_from(identifier);
	return new_scope;
}

jo_scope_t* jo_scope_pop(jo_scope_t* scope)
{
	jo_scope_t* old_scope = scope->parent;
	jo_dyn_array_free(&scope->symbol_table);	
	return old_scope;
}

jo_symbol_t* jo_scope_add_symbol(jo_scope_t* scope, jo_symbol_t symbol)
{
	jo_dyn_array_append(&scope->symbol_table, symbol);

	return &scope->symbol_table.data[scope->symbol_table.occupied - 1];
}