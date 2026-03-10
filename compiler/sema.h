#ifndef jota_sema
#define jota_sema

#include "ast_node.h"
#include "symbol.h"

#define jo_sema_max_err_len 256

typedef struct
{
	char err[jo_sema_max_err_len];
}jo_sema_err_t;

jo_decl_dyn_array_named(jo_sema_err_t, jo_sema_err_dyn_array_t)

typedef struct 
{
	jo_scope_t* scope;
	jo_sema_err_dyn_array_t errors;
} jo_sema_t;


bool jo_sema_analyze(jo_ast_node_t* entry_node);

#endif