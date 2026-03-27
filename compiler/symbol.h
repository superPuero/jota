#ifndef jota_symbol
#define jota_symbol

#include "token.h"

typedef enum 
{
    jo_symbol_kind_variable,
    jo_symbol_kind_function,
    // jo_symbol_kind_type
} jo_symbol_kind;

struct jo_ast_node;
typedef struct jo_ast_node_s jo_ast_node;

typedef struct 
{
	jo_astr identifier;
    jo_symbol_kind kind;    
	jo_u32 location;
	jo_ast_node* ast_node;
} jo_symbol;

jo_ada_declare(jo_symbol, jo_symbol_table);

typedef struct jo_scope jo_scope;

struct jo_scope
{
	jo_astr identifier;
	jo_symbol_table symbol_table;
	jo_scope* parent;
};	

jo_symbol jo_make_symbol(jo_arena* arena,  jo_str_view identifier, jo_symbol_kind kind);
jo_symbol* jo_scope_lookup_symbol(jo_scope* scope, jo_str_view identifier);
jo_scope* jo_scope_push(jo_arena* arena, jo_scope* scope,  jo_str_view identifier);
jo_scope* jo_scope_pop(jo_scope* scope);
jo_symbol* jo_scope_add_symbol(jo_arena* arena, jo_scope* scope, jo_symbol symbol);

#endif