#ifndef jota_symbol
#define jota_symbol

#include "token.h"

typedef enum 
{
    jo_symbol_kind_variable,
    jo_symbol_kind_function,
    // jo_symbol_kind_type
} jo_symbol_kind_t;

typedef struct 
{
    char identifier[jo_token_content_max_length]; 
    jo_symbol_kind_t kind;    
    char asm_location[16]; 
} jo_symbol_t;

jo_decl_dyn_array_named(jo_symbol_t, jo_symbol_table_t);

typedef struct jo_scope jo_scope_t;

struct jo_scope
{
	jo_symbol_table_t symbol_table;
	jo_scope_t* parent;
};	

jo_symbol_t* jo_scope_lookup_symbol(jo_scope_t* scope, const char* identifier);
jo_scope_t* jo_scope_push(jo_scope_t* scope);
jo_scope_t* jo_scope_pop(jo_scope_t* scope);
jo_symbol_t* jo_scope_add_symbol(jo_scope_t* scope, jo_symbol_t symbol);

#endif