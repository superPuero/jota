#include "sema.h"	


jo_symbol_t* jo_sema_lookup_symbol(jo_sema_t* sema, const char* symbol_name)
{
	return jo_scope_lookup_symbol(sema->scope, symbol_name);
}

void jo_sema_push_err(jo_sema_t* sema, const char* err)
{
	jo_sema_err_t err_value = {0};

	memcpy(err_value.err, err, strlen(err));

	jo_dyn_array_append(&sema->errors, err_value);
}

void jo_sema_analyze_declaration(jo_sema_t* sema, jo_ast_node_t* declaration)
{	
	jo_symbol_t* sym = jo_sema_lookup_symbol(sema, declaration->data.declaration.identifier->data.identifier);
	if(sym)
	{
		jo_sema_push_err(sema, "symbol name collision");
	}

}

void jo_sema_walk(jo_sema_t* sema, jo_ast_node_t* node)
{
	if(!node) return;

	switch (node->type)
	{
	case jo_ast_declaration:

		
		break;
	
	default:
		break;
	}
}

bool jo_sema_analyze(jo_ast_node_t* entry_node)
{
	jo_sema_t sema = {0};

	jo_sema_walk(&sema, entry_node);
}


