#ifndef jota_symbol
#define jota_symbol

#include "token.h"

typedef enum 
{
    jo_symbol_kind_variable,
    jo_symbol_kind_function,
    // jo_symbol_kind_type
} jo_symbol_kind_t;

struct jo_ast_node;
typedef struct jo_ast_node jo_ast_node_t;

typedef struct 
{
	jo_astr_t identifier;
    jo_symbol_kind_t kind;    
	jo_u32 location;
	jo_ast_node_t* ast_node;
} jo_symbol_t;

jo_ada_declare(jo_symbol_t, jo_symbol_table_t);

typedef struct jo_scope jo_scope_t;

struct jo_scope
{
	jo_astr_t identifier;
	jo_symbol_table_t symbol_table;
	jo_scope_t* parent;
};	

jo_symbol_t jo_make_symbol(jo_arena_t* arena,  jo_str_view_t identifier, jo_symbol_kind_t kind);
jo_symbol_t* jo_scope_lookup_symbol(jo_scope_t* scope, jo_str_view_t  identifier);
jo_scope_t* jo_scope_push(jo_arena_t* arena, jo_scope_t* scope,  jo_str_view_t identifier);
jo_scope_t* jo_scope_pop(jo_scope_t* scope);
jo_symbol_t* jo_scope_add_symbol(jo_arena_t* arena, jo_scope_t* scope, jo_symbol_t symbol);

#endif