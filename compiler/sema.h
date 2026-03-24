#ifndef jota_sema
#define jota_sema

#include "ast_node.h"
#include "symbol.h"

#define jo_sema_max_err_len 256

// #define jo_sema_report_err(sema_ptr, fmt, ...)\
// jo_dyn_array_append(sema_ptr)->error

typedef struct
{
	char err[jo_sema_max_err_len];
}jo_sema_err_t;

jo_decl_dyn_array_named(jo_sema_err_t, jo_sema_err_dyn_array_t)
jo_decl_dyn_array_named(jo_ast_decl_namespace*, jo_namespace_stack_t)

typedef struct
{
	jo_sema_err_dyn_array_t errors;
	jo_namespace_stack_t namespace_stack;
} jo_sema_t;

bool jo_sema_types_are_equal(jo_ast_node_t* t1, jo_ast_node_t* t2);

void jo_sema_resolve_expr_literal_fn(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* literal_fn_node);
void jo_sema_resolve_expr(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* expr_node);
void jo_sema_resolve_decl(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* decl_node);
void jo_sema_resolve_block(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* block, jo_ast_node_t* literal_parent_fn_node);
void jo_sema_resolve_stmt(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* stmt, jo_ast_node_t* literal_parent_fn_node);
void jo_sema_analyze_literal_fn(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* literal_fn_node);

bool jo_sema_analyze(jo_ast_node_t* entry_node, jo_scope_t* global_scope);

#endif
