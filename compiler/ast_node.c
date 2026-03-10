#include "ast_node.h"

#include "../core/core.h"

const char* jo_ast_node_type_to_stirng(jo_ast_node_type_t type)
{
	switch (type)
	{

		jo_stringify_case(jo_ast_entry);
		jo_stringify_case(jo_ast_type);
		jo_stringify_case(jo_ast_type_primitive);
		jo_stringify_case(jo_ast_type_function);
		jo_stringify_case(jo_ast_type_struct);
		jo_stringify_case(jo_ast_type_tuple);

		jo_stringify_case(jo_ast_literal_integer);
		jo_stringify_case(jo_ast_literal_string);
		jo_stringify_case(jo_ast_literal_bool);

		jo_stringify_case(jo_ast_identifier);	
		jo_stringify_case(jo_ast_declaration);
		jo_stringify_case(jo_ast_construct_variable);
		jo_stringify_case(jo_ast_construct_fn);
		jo_stringify_case(jo_ast_unary_opperation);
		jo_stringify_case(jo_ast_binary_opperation);
		jo_stringify_case(jo_ast_apply_operation);
		jo_stringify_case(jo_ast_expression_if);

		jo_stringify_case(jo_ast_statement_expression);
		jo_stringify_case(jo_ast_statement_declaration);
		jo_stringify_case(jo_ast_statement_return);

		jo_stringify_case(jo_ast_block);
	default:
		break;
	}
}