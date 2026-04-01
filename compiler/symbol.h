#ifndef jota_symbol
#define jota_symbol

#include "compiler_fwd.h"
#include "token.h"

typedef enum 
{
    symbol_kind_variable,
    symbol_kind_function,
    // symbol_kind_type
} symbol_kind;

struct ast_node;
struct symbol_s 
{
	str identifier;
    symbol_kind kind;    
	u32 location;
	ast_node* ast_node;
};

da_declare(symbol, symbol_table);

struct scope_s
{
	str identifier;
	symbol_table symbol_table;
	scope* parent;
};	

symbol make_symbol(arena* arena,  strv identifier, symbol_kind kind);
symbol* scope_lookup_symbol(scope* scope, strv identifier);
scope* scope_push(arena* arena, scope* scope,  strv identifier);
scope* scope_pop(scope* scope);
symbol* scope_add_symbol(arena* arena, scope* scope, symbol symbol);

#endif