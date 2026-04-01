#ifndef jota_sema
#define jota_sema

#include "ast_node.h"
#include "symbol.h"
#include "workspace.h"	

#define sema_max_err_len 256

typedef struct
{
	char err[sema_max_err_len];
}sema_err;

da_declare(sema_err, sema_err_da)

typedef struct
{
	workspace* ws;
	sema_err_da errors;
} sema;

bool8 sema_types_are_equal(ast_node* t1, ast_node* t2);

void sema_resolve_expr_literal_fn(sema* sema, scope* outer_scope, ast_node* literal_fn_node);
void sema_resolve_expr(sema* sema, scope* outer_scope, ast_node* expr_node);
void sema_resolve_decl(sema* sema, scope* outer_scope, ast_node* decl_node);
void sema_resolve_block(sema* sema, scope* outer_scope, ast_node* block, ast_node* literal_parent_fn_node);
void sema_resolve_stmt(sema* sema, scope* outer_scope, ast_node* stmt, ast_node* literal_parent_fn_node);
void sema_analyze_literal_fn(sema* sema, scope* outer_scope, ast_node* literal_fn_node);

bool8 sema_analyze(sema* sema);

#endif
