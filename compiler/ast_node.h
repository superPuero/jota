#ifndef jota_ast_node
#define jota_ast_node

#include "../core/core.h"
#include "compiler_fwd.h"
#include "token.h"
#include "symbol.h"

typedef u32 ast_node_id;

typedef enum
{
	ast_type_file,

	ast_type_directive_intrinsic,

	// ast_type_type_struct,
	ast_type_type_ptr,
	ast_type_type_ref,
	ast_type_type_array,

	ast_type_type_primitive,
	ast_type_type_fn,
	ast_type_type_tuple,
	ast_type_type_struct,
	ast_type_type_type,

	ast_type_literal_u64,
	ast_type_literal_i64,
	ast_type_literal_f64,

	ast_type_literal_string,
	ast_type_literal_bool,

	ast_type_literal_fn,
	ast_type_literal_struct,
	ast_type_literal_import,

	ast_type_literal_type,

	ast_type_identifier,
	ast_type_decl,

	ast_type_decl_namespace,
	ast_type_decl_imports,

	ast_type_stmt_for,
	ast_type_stmt_ifelse,
	ast_type_stmt_expr,
	ast_type_stmt_return,

	ast_type_expr_op_unary,
	ast_type_expr_op_binary,
	ast_type_expr_op_call,
	ast_type_expr_op_index,
	
	ast_type_expr_member_access,
	ast_type_expr_match,

	ast_type_expr_as_cast,

	ast_type_expr_decl,
	ast_type_expr_assignment,
	ast_type_expr_break,
	ast_type_expr_continue,

	ast_type_block
	
} ast_node_type;

const char* ast_node_type_to_stirng(ast_node_type type);

ada_declare(ast_node*, ast_node_ptr_ada);

typedef struct 
{
	ast_node* type_node;
}ast_type_type;

typedef struct
{
	ast_node_ptr_ada members;
	u32 size;
	u32 alignment;
} ast_type_struct;

typedef struct
{
	ast_node_ptr_ada content;
} ast_file;

typedef struct
{
	str_view path;
} ast_literal_import;

typedef struct
{
	ast_node_ptr_ada statements;
}ast_block;

typedef struct
{
	ast_node* expr;
} ast_stmt_expr;

typedef struct
{
	ast_node* condition;
	ast_node* true_block;
	ast_node* tail_stmt_if;
	ast_node* false_block;
} ast_stmt_ifelse;

typedef struct
{
	ast_node* left_expression;
	ast_node* right_expression;
	tok operator_type;
	bool8 self;
} ast_expr_op_binary;

typedef struct
{
	ast_node* expr;
	ast_node* to_type;
	bool8 implicit; 
} ast_expr_as_cast;

typedef struct
{
	ast_node* target;
	str_view member_identifier;
} ast_expr_member_access;

typedef struct
{
	ast_node* target;
	ast_node* expression;
}ast_expr_assignment;

typedef struct
{
	ast_node* expression;
	tok operator_type;
} ast_expr_op_unary;

typedef struct
{
	ast_node* expression;
} ast_stmt_return;

typedef struct
{
	ast_node* iterator;
	ast_node* iterable;
	ast_node* expression;
} ast_stmt_for;

typedef struct
{
	ast_node* expression;
} ast_expr_expression;


typedef struct
{
	ast_node* declaration;
} ast_expr_declaration;

typedef struct
{
	ast_node* target;
	ast_node_ptr_ada arguments;
} ast_expr_op_call;


typedef struct
{
	ast_node* target;
	ast_node_ptr_ada arguments;
} ast_expr_op_index;

typedef struct
{
	ast_node* identifier;
	ast_node* specified_type;
	ast_node* initialize_expression;
	bool8 is_static;
} ast_decl;

typedef struct
{
	ast_node_ptr_ada parameters;
	ast_node* return_type;
	ast_node* block;
	bool8 intrinsic;
} ast_literal_fn;

typedef struct
{
	ast_node_ptr_ada members;
} ast_literal_struct;

typedef struct
{
	ast_node* type;
} ast_literal_type;

typedef struct
{
	ast_node* identifier;
	ast_node_ptr_ada decls_and_directives;
} ast_decl_namespace;

typedef struct
{
	ast_node* inner;
} ast_type_ptr;


typedef struct
{
	ast_node* inner;
} ast_type_ref;


typedef struct
{
	ast_node* inner;
	ast_node* array_size_expression;
} ast_type_array;


typedef struct
{
	ast_node_ptr_ada parameters;
	ast_node* return_type;
} ast_type_fn;

typedef struct
{
	ast_node_ptr_ada entries;
} ast_type_tuple;

struct ast_node_s
{
	ast_node_type type;

	u32 line;
	u32 column;


	// ---- resolved my sema ----
	ast_node* resolved_type;
	symbol* resolved_symbol;

	union
	{
		ast_file file;
		tok type_primitive;

        u64 literal_u64;
        i64 literal_i64;
        f64 literal_f64;

        bool8 literal_bool;

        str_view literal_string;
		
		ast_expr_op_unary 				expr_op_unary;
        ast_expr_op_binary               expr_op_binary;
		ast_expr_op_call					expr_op_call;
		ast_expr_op_index 		        expr_op_index;
		ast_expr_as_cast 				expr_as_cast;

		ast_expr_assignment  	expr_assignment;
		ast_expr_declaration 	expr_decl;

		ast_decl 				decl;

		ast_literal_fn			literal_fn;
		ast_literal_struct 		literal_struct;
		ast_literal_import 		literal_import;
		ast_literal_type 		literal_type;

        str_view				identifier;

		ast_decl_namespace 	decl_namesapce;

		ast_block 			block;

		ast_stmt_ifelse 		stmt_ifelse;
		ast_stmt_expr 		stmt_expr;
		ast_stmt_return 		stmt_return;
		ast_stmt_for 		stmt_for;


		ast_type_ptr 		type_ptr;
		ast_type_ref 		type_ref;
		ast_type_array 		type_array;
		ast_type_fn 			type_fn;
		ast_type_tuple 		type_tuple;
		ast_type_struct 		type_struct;
		ast_type_type 		type_type;
    } data;
};

void dump_ast_node(ast_node* node, u32 indent);
ast_node* ast_node_make(arena* arena, ast_node_type type);

#endif
