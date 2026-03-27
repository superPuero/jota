#include "ast_node.h"

#include "../core/core.h"
#include <stdio.h>


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


static void print_indent(jo_u32 indent) {
    for (jo_u32 i = 0; i < indent; i++) 
	{
        printf("  ");
    }
}

void jo_dump_ast_node(jo_ast_node_t* node, jo_u32 indent)
{
	if (!node) 
	{
        print_indent(indent);
        printf("<NULL>\n");
        return;
    }

	print_indent(indent);

    printf("[%s]", jo_ast_node_type_to_stirng(node->type));

	switch (node->type)
	{	
	case jo_ast_type_literal_u64:
		printf(" %llu\n", node->data.literal_u64);
		break;
	case jo_ast_type_literal_i64:
		printf(" %lli\n", node->data.literal_i64);
		break;
	case jo_ast_type_literal_f64:
		printf(" %f\n", node->data.literal_f64);
		break;
	case jo_ast_type_literal_bool:
		printf(" %s\n", node->data.literal_bool ? "true" : "false");
		break;
	case jo_ast_type_literal_type:
		printf("\n");
		jo_dump_ast_node(node->data.literal_type.type, indent + 1);
		break;
	case jo_ast_type_literal_string:
		printf(" \"%s\"\n", node->data.literal_string.data);
		break;
	case jo_ast_type_identifier:
		printf(" %s\n", node->data.identifier.data);
		break;
	case jo_ast_type_type_primitive:
		printf(" (%s)\n", jo_token_type_to_string(node->data.type_primitive));
		break;

	case jo_ast_type_stmt_expr:
		printf("\n");
		jo_dump_ast_node(node->data.stmt_expr.expr, indent + 1);
		break;

	case jo_ast_type_module:
		printf("\n");
		jo_ada_foreach(&node->data.module.content)
		{
			jo_dump_ast_node(*node->data.module.content.it, indent + 1);
		};
		break;
	case jo_ast_type_decl_namespace:
		printf("\n");
		jo_ada_foreach(&node->data.decl_namesapce.decls_and_directives)
		{
			jo_dump_ast_node(*node->data.decl_namesapce.decls_and_directives.it, indent + 1);
		};
		break;

	case jo_ast_type_type_ptr:
		printf("\n");
		jo_dump_ast_node(node->data.type_ptr.inner, indent + 1);
		break;
	case jo_ast_type_type_ref:
		printf("\n");
		jo_dump_ast_node(node->data.type_ref.inner, indent + 1);
		break;
	case jo_ast_type_type_array:
		printf("\n");
		jo_dump_ast_node(node->data.type_array.inner, indent + 1);
		jo_dump_ast_node(node->data.type_array.array_size_expression, indent + 1);
		break;
	case jo_ast_type_type_fn:
		printf("\n");
		jo_ada_foreach(&node->data.type_fn.parameters)
		{
			jo_dump_ast_node(*node->data.type_fn.parameters.it, indent + 1);
		};
		jo_dump_ast_node(node->data.type_fn.return_type, indent + 1);
		break;
	case jo_ast_type_type_tuple:
		printf("\n");
		jo_ada_foreach(&node->data.type_tuple.entries)
		{
			jo_dump_ast_node(*node->data.type_tuple.entries.it, indent + 1);
		};
		break;

	case jo_ast_type_decl:
		printf("\n");
		jo_dump_ast_node(node->data.decl.identifier, indent + 1);
		jo_dump_ast_node(node->data.decl.specified_type, indent + 1);
		jo_dump_ast_node(node->data.decl.initialize_expression, indent + 1);
		break;
	case jo_ast_type_literal_fn:
		printf("\n");
		jo_ada_foreach(&node->data.literal_fn.parameters)
		{
			jo_dump_ast_node(*node->data.literal_fn.parameters.it, indent + 1);
		};
		jo_dump_ast_node(node->data.literal_fn.return_type, indent + 1);
		jo_dump_ast_node(node->data.literal_fn.block, indent + 1);
		break;
	case jo_ast_type_literal_struct:
		printf("\n");
		jo_ada_foreach(&node->data.literal_struct.members)
		{
			jo_dump_ast_node(*node->data.literal_struct.members.it, indent + 1);
		};
			break;

	// --- Expressions ---
	case jo_ast_type_expr_op_unary:
		printf(" (Op: %s)\n", jo_token_type_to_string(node->data.expr_op_unary.operator_type));
		jo_dump_ast_node(node->data.expr_op_unary.expression, indent + 1);
		break;
	case jo_ast_type_expr_op_binary:
		printf(" (Op: %s)\n", jo_token_type_to_string(node->data.expr_op_binary.operator_type));
		jo_dump_ast_node(node->data.expr_op_binary.left_expression, indent + 1);
		jo_dump_ast_node(node->data.expr_op_binary.right_expression, indent + 1);
		break;
	case jo_ast_type_expr_op_call:
		printf("\n");
		jo_dump_ast_node(node->data.expr_op_call.target, indent + 1);
		jo_ada_foreach(&node->data.expr_op_call.arguments)
		{
			jo_dump_ast_node(*node->data.expr_op_call.arguments.it, indent + 1);
		};
		break;
	case jo_ast_type_expr_op_index:
		printf("\n");
		jo_dump_ast_node(node->data.expr_op_index.target, indent + 1);
		jo_ada_foreach(&node->data.expr_op_index.arguments)
		{
			jo_dump_ast_node(*node->data.expr_op_index.arguments.it, indent + 1);
		};
		break;
	case jo_ast_type_stmt_ifelse:
		printf("\n");
		jo_dump_ast_node(node->data.stmt_ifelse.condition, indent + 1);
		jo_dump_ast_node(node->data.stmt_ifelse.true_block, indent + 1);
		jo_dump_ast_node(node->data.stmt_ifelse.tail_stmt_if, indent + 1);
		jo_dump_ast_node(node->data.stmt_ifelse.false_block, indent + 1);
		break;

	case jo_ast_type_expr_decl:
		printf("\n");
		jo_dump_ast_node(node->data.expr_decl.declaration, indent + 1);
		break;
		case jo_ast_type_expr_as_cast:
		printf("\n");
		jo_dump_ast_node(node->data.expr_as_cast.expr, indent + 1);
		jo_dump_ast_node(node->data.expr_as_cast.to_type, indent + 1);
		break;
	case jo_ast_type_stmt_return:
		printf("\n");
		jo_dump_ast_node(node->data.stmt_return.expression, indent + 1);
		break;
	case jo_ast_type_stmt_for:
		printf("\n");
		jo_dump_ast_node(node->data.stmt_for.iterator, indent + 1);
		jo_dump_ast_node(node->data.stmt_for.iterable, indent + 1);
		jo_dump_ast_node(node->data.stmt_for.expression, indent + 1);
		break;
	case jo_ast_type_block:
		printf("\n");
		jo_ada_foreach(&node->data.block.statements)
		{
			jo_dump_ast_node(*node->data.block.statements.it, indent + 1);
		};
		break;
	case jo_ast_type_directive_load:
		printf(" %s\n", node->data.directive_load.path.data);
		break;

	default:
		printf(" <unmapped data>\n");
		break;
	}
}

jo_ast_node_t* jo_ast_node_make(jo_arena_t* arena, jo_ast_node_type_t type)
{
	jo_ast_node_t* node = jo_arena_palloc(arena, jo_ast_node_t);
	node->type = type;
	return node;
}