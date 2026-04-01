#include "ast_node.h"

#include "../core/core.h"
#include <stdio.h>


const char* ast_node_type_to_stirng(ast_node_type type)
{
	switch (type)
	{
		stringify_case(ast_type_file);

		stringify_case(ast_type_type_ptr);
		stringify_case(ast_type_type_ref);
		stringify_case(ast_type_type_array);
		stringify_case(ast_type_type_primitive);
		stringify_case(ast_type_type_fn);
		stringify_case(ast_type_type_tuple);
		stringify_case(ast_type_type_struct);
		stringify_case(ast_type_type_type);

		stringify_case(ast_type_literal_u64);
		stringify_case(ast_type_literal_i64);
		stringify_case(ast_type_literal_f64);

		stringify_case(ast_type_literal_string);
		stringify_case(ast_type_literal_bool);
		stringify_case(ast_type_literal_type);
		stringify_case(ast_type_block);

		stringify_case(ast_type_stmt_ifelse);
		stringify_case(ast_type_stmt_expr);
		stringify_case(ast_type_stmt_return);
		stringify_case(ast_type_stmt_for);

		stringify_case(ast_type_identifier);
		stringify_case(ast_type_decl);
		stringify_case(ast_type_literal_fn);
		stringify_case(ast_type_literal_struct);
		stringify_case(ast_type_decl_namespace);
		stringify_case(ast_type_expr_op_unary);
		stringify_case(ast_type_expr_op_binary);
		stringify_case(ast_type_expr_op_call);
		stringify_case(ast_type_expr_op_index);
		stringify_case(ast_type_expr_as_cast);
		stringify_case(ast_type_expr_member_access);
		stringify_case(ast_type_expr_match);
		stringify_case(ast_type_expr_decl);
		stringify_case(ast_type_expr_break);
		stringify_case(ast_type_expr_continue);


	default:
		exit(1);
		break;
	}
}


static void print_indent(u32 indent) {
    for (u32 i = 0; i < indent; i++) 
	{
        printf("  ");
    }
}

void dump_ast_node(ast_node* node, u32 indent)
{
	if (!node) 
	{
        print_indent(indent);
        printf("<NULL>\n");
        return;
    }

	print_indent(indent);

    printf("[%s]", ast_node_type_to_stirng(node->type));

	switch (node->type)
	{	
	case ast_type_literal_u64:
		printf(" %llu\n", node->data.literal_u64);
		break;
	case ast_type_literal_i64:
		printf(" %lli\n", node->data.literal_i64);
		break;
	case ast_type_literal_f64:
		printf(" %f\n", node->data.literal_f64);
		break;
	case ast_type_literal_bool:
		printf(" %s\n", node->data.literal_bool ? "true" : "false");
		break;
	case ast_type_literal_type:
		printf("\n");
		dump_ast_node(node->data.literal_type.type, indent + 1);
		break;
	case ast_type_literal_string:
		printf(" \"%.*s\"\n", str_view_fmt(&node->data.literal_string));
		break;
	case ast_type_identifier:
		printf(" %.*s\n", str_view_fmt(&node->data.identifier));
		break;
	case ast_type_type_primitive:
		printf(" (%s)\n", tok_to_string(node->data.type_primitive));
		break;

	case ast_type_stmt_expr:
		printf("\n");
		dump_ast_node(node->data.stmt_expr.expr, indent + 1);
		break;

	case ast_type_file:
		printf("\n");
		ada_foreach(&node->data.file.content)
		{
			dump_ast_node(*node->data.file.content.it, indent + 1);
		};
		break;
	case ast_type_decl_namespace:
		printf("\n");
		ada_foreach(&node->data.decl_namesapce.decls_and_directives)
		{
			dump_ast_node(*node->data.decl_namesapce.decls_and_directives.it, indent + 1);
		};
		break;

	case ast_type_type_ptr:
		printf("\n");
		dump_ast_node(node->data.type_ptr.inner, indent + 1);
		break;
	case ast_type_type_ref:
		printf("\n");
		dump_ast_node(node->data.type_ref.inner, indent + 1);
		break;
	case ast_type_type_array:
		printf("\n");
		dump_ast_node(node->data.type_array.inner, indent + 1);
		dump_ast_node(node->data.type_array.array_size_expression, indent + 1);
		break;
	case ast_type_type_fn:
		printf("\n");
		ada_foreach(&node->data.type_fn.parameters)
		{
			dump_ast_node(*node->data.type_fn.parameters.it, indent + 1);
		};
		dump_ast_node(node->data.type_fn.return_type, indent + 1);
		break;
	case ast_type_type_tuple:
		printf("\n");
		ada_foreach(&node->data.type_tuple.entries)
		{
			dump_ast_node(*node->data.type_tuple.entries.it, indent + 1);
		};
		break;

	case ast_type_decl:
		printf("\n");
		dump_ast_node(node->data.decl.identifier, indent + 1);
		dump_ast_node(node->data.decl.specified_type, indent + 1);
		dump_ast_node(node->data.decl.initialize_expression, indent + 1);
		break;
	case ast_type_literal_fn:
		printf("\n");
		ada_foreach(&node->data.literal_fn.parameters)
		{
			dump_ast_node(*node->data.literal_fn.parameters.it, indent + 1);
		};
		dump_ast_node(node->data.literal_fn.return_type, indent + 1);
		dump_ast_node(node->data.literal_fn.block, indent + 1);
		break;
	case ast_type_literal_struct:
		printf("\n");
		ada_foreach(&node->data.literal_struct.members)
		{
			dump_ast_node(*node->data.literal_struct.members.it, indent + 1);
		};
			break;

	// --- Expressions ---
	case ast_type_expr_op_unary:
		printf(" (Op: %s)\n", tok_to_string(node->data.expr_op_unary.operator_type));
		dump_ast_node(node->data.expr_op_unary.expression, indent + 1);
		break;
	case ast_type_expr_op_binary:
		printf(" (Op: %s)\n", tok_to_string(node->data.expr_op_binary.operator_type));
		dump_ast_node(node->data.expr_op_binary.left_expression, indent + 1);
		dump_ast_node(node->data.expr_op_binary.right_expression, indent + 1);
		break;
	case ast_type_expr_op_call:
		printf("\n");
		dump_ast_node(node->data.expr_op_call.target, indent + 1);
		ada_foreach(&node->data.expr_op_call.arguments)
		{
			dump_ast_node(*node->data.expr_op_call.arguments.it, indent + 1);
		};
		break;
	case ast_type_expr_op_index:
		printf("\n");
		dump_ast_node(node->data.expr_op_index.target, indent + 1);
		ada_foreach(&node->data.expr_op_index.arguments)
		{
			dump_ast_node(*node->data.expr_op_index.arguments.it, indent + 1);
		};
		break;
	case ast_type_stmt_ifelse:
		printf("\n");
		dump_ast_node(node->data.stmt_ifelse.condition, indent + 1);
		dump_ast_node(node->data.stmt_ifelse.true_block, indent + 1);
		dump_ast_node(node->data.stmt_ifelse.tail_stmt_if, indent + 1);
		dump_ast_node(node->data.stmt_ifelse.false_block, indent + 1);
		break;

	case ast_type_expr_decl:
		printf("\n");
		dump_ast_node(node->data.expr_decl.declaration, indent + 1);
		break;
		case ast_type_expr_as_cast:
		printf("\n");
		dump_ast_node(node->data.expr_as_cast.expr, indent + 1);
		dump_ast_node(node->data.expr_as_cast.to_type, indent + 1);
		break;
	case ast_type_stmt_return:
		printf("\n");
		dump_ast_node(node->data.stmt_return.expression, indent + 1);
		break;
	case ast_type_stmt_for:
		printf("\n");
		dump_ast_node(node->data.stmt_for.iterator, indent + 1);
		dump_ast_node(node->data.stmt_for.iterable, indent + 1);
		dump_ast_node(node->data.stmt_for.expression, indent + 1);
		break;
	case ast_type_block:
		printf("\n");
		ada_foreach(&node->data.block.statements)
		{
			dump_ast_node(*node->data.block.statements.it, indent + 1);
		};
		break;
	default:
		printf(" <unmapped data>\n");
		break;
	}
}

ast_node* ast_node_make(arena* arena, ast_node_type type)
{
	ast_node* node = arena_ppush(arena, ast_node);
	node->type = type;
	return node;
}