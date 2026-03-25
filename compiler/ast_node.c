#include "ast_node.h"

#include "../core/core.h"

const char* jo_ast_node_type_to_stirng(jo_ast_node_type_t type)
{
	switch (type)
	{
		jo_stringify_case(jo_ast_type_module);

		jo_stringify_case(jo_ast_type_directive_load);
		jo_stringify_case(jo_ast_type_type_ptr);
		jo_stringify_case(jo_ast_type_type_ref);
		jo_stringify_case(jo_ast_type_type_array);
		jo_stringify_case(jo_ast_type_type_primitive);
		jo_stringify_case(jo_ast_type_type_fn);
		jo_stringify_case(jo_ast_type_type_tuple);
		jo_stringify_case(jo_ast_type_type_struct);
		jo_stringify_case(jo_ast_type_type_type);

		jo_stringify_case(jo_ast_type_literal_u64);
		jo_stringify_case(jo_ast_type_literal_i64);

		jo_stringify_case(jo_ast_type_literal_u32);
		jo_stringify_case(jo_ast_type_literal_i32);

		jo_stringify_case(jo_ast_type_literal_u16);
		jo_stringify_case(jo_ast_type_literal_i16);

		jo_stringify_case(jo_ast_type_literal_u8);
		jo_stringify_case(jo_ast_type_literal_i8);

		jo_stringify_case(jo_ast_type_literal_f32);
		jo_stringify_case(jo_ast_type_literal_f64);

		jo_stringify_case(jo_ast_type_literal_string);
		jo_stringify_case(jo_ast_type_literal_bool);
		jo_stringify_case(jo_ast_type_literal_type);
		jo_stringify_case(jo_ast_type_block);

		jo_stringify_case(jo_ast_type_stmt_ifelse);
		jo_stringify_case(jo_ast_type_stmt_expr);
		jo_stringify_case(jo_ast_type_stmt_return);
		jo_stringify_case(jo_ast_type_stmt_for);

		jo_stringify_case(jo_ast_type_identifier);
		jo_stringify_case(jo_ast_type_decl);
		jo_stringify_case(jo_ast_type_literal_fn);
		jo_stringify_case(jo_ast_type_literal_struct);
		jo_stringify_case(jo_ast_type_decl_namespace);
		jo_stringify_case(jo_ast_type_expr_op_unary);
		jo_stringify_case(jo_ast_type_expr_op_binary);
		jo_stringify_case(jo_ast_type_expr_op_call);
		jo_stringify_case(jo_ast_type_expr_op_index);
		jo_stringify_case(jo_ast_type_expr_as_cast);
		jo_stringify_case(jo_ast_type_expr_member_access);
		jo_stringify_case(jo_ast_type_expr_match);
		jo_stringify_case(jo_ast_type_expr_decl);
		jo_stringify_case(jo_ast_type_expr_break);
		jo_stringify_case(jo_ast_type_expr_continue);


	default:
		exit(1);
		break;
	}
}
