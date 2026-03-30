#ifndef jota_ast_node
#define jota_ast_node

#include "../core/core.h"
#include "token.h"
#include "symbol.h"

typedef jo_u32 jo_ast_node_id;

typedef enum
{
	jo_ast_type_file,

	jo_ast_type_directive_intrinsic,

	// jo_ast_type_type_struct,
	jo_ast_type_type_ptr,
	jo_ast_type_type_ref,
	jo_ast_type_type_array,

	jo_ast_type_type_primitive,
	jo_ast_type_type_fn,
	jo_ast_type_type_tuple,
	jo_ast_type_type_struct,
	jo_ast_type_type_type,

	jo_ast_type_literal_u64,
	jo_ast_type_literal_i64,
	jo_ast_type_literal_f64,

	jo_ast_type_literal_string,
	jo_ast_type_literal_bool,

	jo_ast_type_literal_fn,
	jo_ast_type_literal_struct,
	jo_ast_type_literal_import,

	jo_ast_type_literal_type,

	jo_ast_type_identifier,
	jo_ast_type_decl,

	jo_ast_type_decl_namespace,
	jo_ast_type_decl_imports,

	jo_ast_type_stmt_for,
	jo_ast_type_stmt_ifelse,
	jo_ast_type_stmt_expr,
	jo_ast_type_stmt_return,

	jo_ast_type_expr_op_unary,
	jo_ast_type_expr_op_binary,
	jo_ast_type_expr_op_call,
	jo_ast_type_expr_op_index,
	
	jo_ast_type_expr_member_access,
	jo_ast_type_expr_match,

	jo_ast_type_expr_as_cast,

	jo_ast_type_expr_decl,
	jo_ast_type_expr_assignment,
	jo_ast_type_expr_break,
	jo_ast_type_expr_continue,

	jo_ast_type_block
} jo_ast_node_type;

const char* jo_ast_node_type_to_stirng(jo_ast_node_type type);

typedef struct jo_ast_node_s jo_ast_node;

jo_ada_declare(jo_ast_node*, jo_ast_node_ptr_ada);

typedef struct 
{
	jo_ast_node* type_node;
}jo_ast_type_type;

typedef struct
{
	jo_ast_node_ptr_ada members;
	jo_u32 size;
	jo_u32 alignment;
} jo_ast_type_struct;

typedef struct
{
	jo_ast_node_ptr_ada content;
} jo_ast_file;

typedef struct
{
	jo_str_view path;
} jo_ast_literal_import;

typedef struct
{
	jo_ast_node_ptr_ada statements;
}jo_ast_block;

typedef struct
{
	jo_ast_node* expr;
} jo_ast_stmt_expr;

typedef struct
{
	jo_ast_node* condition;
	jo_ast_node* true_block;
	jo_ast_node* tail_stmt_if;
	jo_ast_node* false_block;
} jo_ast_stmt_ifelse;

typedef struct
{
	jo_ast_node* left_expression;
	jo_ast_node* right_expression;
	jo_tok operator_type;
	bool self;
} jo_ast_expr_op_binary;

typedef struct
{
	jo_ast_node* expr;
	jo_ast_node* to_type;
	bool implicit; 
} jo_ast_expr_as_cast;

typedef struct
{
	jo_ast_node* target;
	jo_str_view member_identifier;
} jo_ast_expr_member_access;

typedef struct
{
	jo_ast_node* target;
	jo_ast_node* expression;
}jo_ast_expr_assignment;

typedef struct
{
	jo_ast_node* expression;
	jo_tok operator_type;
} jo_ast_expr_op_unary;

typedef struct
{
	jo_ast_node* expression;
} jo_ast_stmt_return;

typedef struct
{
	jo_ast_node* iterator;
	jo_ast_node* iterable;
	jo_ast_node* expression;
} jo_ast_stmt_for;

typedef struct
{
	jo_ast_node* expression;
} jo_ast_expr_expression;


typedef struct
{
	jo_ast_node* declaration;
} jo_ast_expr_declaration;

typedef struct
{
	jo_ast_node* target;
	jo_ast_node_ptr_ada arguments;
} jo_ast_expr_op_call;


typedef struct
{
	jo_ast_node* target;
	jo_ast_node_ptr_ada arguments;
} jo_ast_expr_op_index;

typedef struct
{
	jo_ast_node* identifier;
	jo_ast_node* specified_type;
	jo_ast_node* initialize_expression;
	bool is_static;
} jo_ast_decl;

typedef struct
{
	jo_ast_node_ptr_ada parameters;
	jo_ast_node* return_type;
	jo_ast_node* block;
	bool intrinsic;
} jo_ast_literal_fn;

typedef struct
{
	jo_ast_node_ptr_ada members;
} jo_ast_literal_struct;

typedef struct
{
	jo_ast_node* type;
} jo_ast_literal_type;

typedef struct
{
	jo_ast_node* identifier;
	jo_ast_node_ptr_ada decls_and_directives;
} jo_ast_decl_namespace;

typedef struct
{
	jo_ast_node* inner;
} jo_ast_type_ptr;


typedef struct
{
	jo_ast_node* inner;
} jo_ast_type_ref;


typedef struct
{
	jo_ast_node* inner;
	jo_ast_node* array_size_expression;
} jo_ast_type_array;


typedef struct
{
	jo_ast_node_ptr_ada parameters;
	jo_ast_node* return_type;
} jo_ast_type_fn;

typedef struct
{
	jo_ast_node_ptr_ada entries;
} jo_ast_type_tuple;

struct jo_ast_node_s
{
	jo_ast_node_type type;

	jo_u32 line;
	jo_u32 column;


	// ---- resolved my sema ----
	jo_ast_node* resolved_type;
	jo_symbol* resolved_symbol;

	union
	{
		jo_ast_file file;
		jo_tok type_primitive;

        jo_u64 literal_u64;
        jo_i64 literal_i64;
        jo_f64 literal_f64;

        bool literal_bool;

        jo_str_view literal_string;
		
		jo_ast_expr_op_unary 				expr_op_unary;
        jo_ast_expr_op_binary               expr_op_binary;
		jo_ast_expr_op_call					expr_op_call;
		jo_ast_expr_op_index 		        expr_op_index;
		jo_ast_expr_as_cast 				expr_as_cast;

		jo_ast_expr_assignment  	expr_assignment;
		jo_ast_expr_declaration 	expr_decl;

		jo_ast_decl 				decl;

		jo_ast_literal_fn			literal_fn;
		jo_ast_literal_struct 		literal_struct;
		jo_ast_literal_import 		literal_import;
		jo_ast_literal_type 		literal_type;

        jo_str_view				identifier;

		jo_ast_decl_namespace 	decl_namesapce;

		jo_ast_block 			block;

		jo_ast_stmt_ifelse 		stmt_ifelse;
		jo_ast_stmt_expr 		stmt_expr;
		jo_ast_stmt_return 		stmt_return;
		jo_ast_stmt_for 		stmt_for;


		jo_ast_type_ptr 		type_ptr;
		jo_ast_type_ref 		type_ref;
		jo_ast_type_array 		type_array;
		jo_ast_type_fn 			type_fn;
		jo_ast_type_tuple 		type_tuple;
		jo_ast_type_struct 		type_struct;
		jo_ast_type_type 		type_type;
    } data;
};

void jo_dump_ast_node(jo_ast_node* node, jo_u32 indent);
jo_ast_node* jo_ast_node_make(jo_arena* arena, jo_ast_node_type type);

#endif
