#ifndef jota_sema
#define jota_sema

#include "ast_node.h"
#include "symbol.h"
#include "workspace.h"	

#define jo_sema_max_err_len 256

typedef struct
{
	char err[jo_sema_max_err_len];
}jo_sema_err;

jo_ada_declare(jo_sema_err, jo_sema_err_ada)

typedef struct
{
	jo_workspace* ws;
	jo_sema_err_ada errors;
} jo_sema;

bool jo_sema_types_are_equal(jo_ast_node* t1, jo_ast_node* t2);

void jo_sema_resolve_expr_literal_fn(jo_sema* sema, jo_scope* outer_scope, jo_ast_node* literal_fn_node);
void jo_sema_resolve_expr(jo_sema* sema, jo_scope* outer_scope, jo_ast_node* expr_node);
void jo_sema_resolve_decl(jo_sema* sema, jo_scope* outer_scope, jo_ast_node* decl_node);
void jo_sema_resolve_block(jo_sema* sema, jo_scope* outer_scope, jo_ast_node* block, jo_ast_node* literal_parent_fn_node);
void jo_sema_resolve_stmt(jo_sema* sema, jo_scope* outer_scope, jo_ast_node* stmt, jo_ast_node* literal_parent_fn_node);
void jo_sema_analyze_literal_fn(jo_sema* sema, jo_scope* outer_scope, jo_ast_node* literal_fn_node);

bool jo_sema_analyze(jo_sema* sema);

#endif
