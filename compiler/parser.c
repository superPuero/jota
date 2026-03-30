#include "parser.h"
#include "token.h"
#include "workspace.h"
#include <errno.h>
#include <stdio.h>

jo_token* jo_parser_peek(jo_parser* parser, jo_u32 offset)
{
	if(parser->current_token + offset < parser->tokens->occupied)
	{
		return parser->tokens->data + parser->current_token + offset;
	}

	assert(0 && "unexpected end %s");
}

jo_token* jo_parser_peek_next(jo_parser* parser)
{
	return jo_parser_peek(parser, 1);
}

void jo_parser_advance(jo_parser* parser)
{
	parser->current_token++;
}

jo_token* jo_parser_current(jo_parser* parser)
{
	// dump_tokens_2(parser->lexer);
	return parser->tokens->data + parser->current_token;
}

void jo_parser_unexpected(jo_parser* parser, const char* err)
{	
	printf("unexpected: %s at line %d column %d got %s", err, jo_parser_current(parser)->line, jo_parser_current(parser)->column, jo_tok_to_string(jo_parser_current(parser)->type));
	assert(0);
}

jo_token* jo_parser_consume(jo_parser* parser, jo_tok expected)
{
    jo_token* current = jo_parser_current(parser);
    if(current->type != expected)
    {
        printf("unexpected token %s, expected %s at line %d column %d",
            jo_tok_to_string(current->type),
            jo_tok_to_string(expected),
            current->line,
			current->column);

		assert(0);
    }

	jo_parser_advance(parser);
	return current;
}

jo_ast_node_ptr_ada jo_parse_type_function_parameters(jo_parser* parser)
{
	jo_ast_node_ptr_ada parameter_nodes = {0};

	jo_parser_consume(parser, jo_tok_open_parenthesis);


	while(true)
	{
		if(jo_parser_current(parser)->type == jo_tok_minus) // minus from return separator arrow (->)
		{
			break;
		}

		if(jo_parser_current(parser)->type ==  jo_tok_close_parenthesis) // implicid -> void
		{
			break;
		}

		if(parameter_nodes.occupied > 0)
		{
			jo_parser_consume(parser, jo_tok_comma);
		}

		jo_ada_append(parser->arena,
			&parameter_nodes,
			jo_parse_declaration(parser)
		);
	}


	return parameter_nodes;
}

jo_ast_node* jo_parse_type_primitive(jo_parser* parser)
{
	jo_ast_node* type_primitive_node = jo_ast_node_make(parser->arena, jo_ast_type_type_primitive);
	type_primitive_node->data.type_primitive = jo_parser_current(parser)->type;
	jo_parser_advance(parser);
	return type_primitive_node;
}

jo_ast_node* jo_parse_type(jo_parser* parser)
{
	if(jo_tok_is_type_primitive(jo_parser_current(parser)->type)){ return jo_parse_type_primitive(parser); }

	jo_ast_node* type_node = {0};
	switch (jo_parser_current(parser)->type)
	{
	case jo_tok_open_square_bracket:
		jo_parser_consume(parser, jo_tok_open_square_bracket);
		type_node = jo_ast_node_make(parser->arena, jo_ast_type_type_array);
		if(jo_parser_current(parser)->type != jo_tok_close_square_bracket)
		{
			type_node->data.type_array.array_size_expression = jo_parse_expression(parser);
		}
		jo_parser_consume(parser, jo_tok_close_square_bracket);
		type_node->data.type_ptr.inner = jo_parse_type(parser);
		break;

	case jo_tok_star:
		jo_parser_consume(parser, jo_tok_star);
		type_node = jo_ast_node_make(parser->arena, jo_ast_type_type_ptr);
		type_node->data.type_ptr.inner = jo_parse_type(parser);
		break;

	case jo_tok_ampersand:
		jo_parser_consume(parser, jo_tok_ampersand);
		type_node = jo_ast_node_make(parser->arena, jo_ast_type_type_ref);
		type_node->data.type_ref.inner = jo_parse_type(parser);
		break;

	case jo_tok_identifier:
		type_node = jo_parse_identifier(parser);
		break;

	case jo_tok_fn:
		type_node = jo_parse_type_fn(parser);
		break;
	default:
		assert(0);
		break;
	}
	//------------------------------------------------------------------

	if(!type_node)
	{
		jo_parser_unexpected(parser, "expected type");
	}

	return type_node;
}

jo_ast_node_ptr_ada jo_parse_expression_list(jo_parser* parser)
{
	jo_ast_node_ptr_ada expression_list = {0};
	while(true)
	{
		if(expression_list.occupied > 0)
		{
			if(jo_parser_current(parser)->type == jo_tok_comma)
			{
				jo_parser_consume(parser, jo_tok_comma);
			}
			else
			{
				return expression_list;
			}
		}

		jo_ada_append(parser->arena,&expression_list, jo_parse_expression(parser));
	}
}

jo_ast_node_ptr_ada jo_parse_type_list(jo_parser* parser)
{
	jo_ast_node_ptr_ada type_list = {0};

	while(true)
	{
		if(type_list.occupied > 0)
		{
			if(jo_parser_current(parser)->type == jo_tok_comma)
			{
				jo_parser_consume(parser, jo_tok_comma);
			}
			else
			{
				return type_list;
			}
		}

		jo_ada_append(parser->arena,&type_list, jo_parse_type(parser));
	}
}

jo_ast_node_ptr_ada jo_parse_declaration_list(jo_parser* parser)
{
	jo_ast_node_ptr_ada declaration_list = {0};

	while(true)
	{
		if(declaration_list.occupied > 0)
		{
			if(jo_parser_current(parser)->type == jo_tok_comma)
			{
				jo_parser_consume(parser, jo_tok_comma);
			}
			else
			{
				return declaration_list;
			}
		}

		jo_ada_append(parser->arena,&declaration_list, jo_parse_declaration(parser));
	}
}

jo_ast_node* jo_try_parse_expression_postfix_operator(jo_parser* parser, jo_ast_node* expression)
{
	switch (jo_parser_current(parser)->type)
	{
	case jo_tok_open_parenthesis:
		jo_parser_consume(parser, jo_tok_open_parenthesis);
		jo_ast_node* apply_paren_operation_node = jo_ast_node_make(parser->arena, jo_ast_type_expr_op_call);
		apply_paren_operation_node->data.expr_op_call.target = expression;
		if(jo_parser_current(parser)->type != jo_tok_close_parenthesis)
		{
			apply_paren_operation_node->data.expr_op_call.arguments = jo_parse_expression_list(parser);
		}
		jo_parser_consume(parser, jo_tok_close_parenthesis);
		return apply_paren_operation_node;
		break;

	case jo_tok_open_square_bracket:
		jo_parser_consume(parser, jo_tok_open_square_bracket);
		jo_ast_node* apply_squaer_brackets_operation_node = jo_ast_node_make(parser->arena, jo_ast_type_expr_op_index);
		apply_squaer_brackets_operation_node->data.expr_op_index.target = expression;
		if(jo_parser_current(parser)->type != jo_tok_close_square_bracket)
		{
			apply_squaer_brackets_operation_node->data.expr_op_index.arguments = jo_parse_expression_list(parser);
		}
		jo_parser_consume(parser, jo_tok_close_square_bracket);
		return apply_squaer_brackets_operation_node;
		break;

	case jo_tok_as:
		jo_parser_consume(parser, jo_tok_as);
		jo_ast_node* as_cast_node = jo_ast_node_make(parser->arena, jo_ast_type_expr_as_cast);
		as_cast_node->data.expr_as_cast.to_type = jo_parse_type(parser);
		as_cast_node->data.expr_as_cast.expr = expression;
		return as_cast_node;
		break;

	default:
		return NULL;
		break;
	}
}

jo_ast_node* jo_parse_expression_precedented(jo_parser* parser, jo_u32 min_precedence)
{
	jo_ast_node* left_expression_node = jo_parse_primary_expression(parser);

	while(true)
	{
		jo_ast_node* postfix_expression = NULL;

		do{
			postfix_expression = jo_try_parse_expression_postfix_operator(parser, left_expression_node);
			if(postfix_expression) { left_expression_node = postfix_expression; }
		}while(postfix_expression);

		jo_tok operator_type = jo_parser_current(parser)->type;
		if(!jo_tok_is_operator(operator_type)) break; // not an operator, statemt end

		jo_u32 precedence = jo_tok_binary_operator_precedence(operator_type);
		if(precedence < min_precedence)
		{
			break;
		}

		jo_parser_advance(parser);

		jo_ast_node* right_expression_node = jo_parse_expression_precedented(parser, precedence + 1);

		jo_ast_node* new_left_expression_node = jo_ast_node_make(parser->arena, jo_ast_type_expr_op_binary);

		new_left_expression_node->data.expr_op_binary.operator_type = operator_type;
		new_left_expression_node->data.expr_op_binary.left_expression = left_expression_node;
		new_left_expression_node->data.expr_op_binary.right_expression = right_expression_node;

		left_expression_node = new_left_expression_node;
	}


	return left_expression_node;
}


jo_ast_node* jo_parse_expression(jo_parser* parser)
{
	return jo_parse_expression_precedented(parser, 0);
}

jo_ast_node* jo_parse_literal_expression(jo_parser* parser)
{
	jo_ast_node* literal_node = NULL;
	char* end;

	switch (jo_parser_current(parser)->type)
	{
		case jo_tok_literal_string:
			literal_node = jo_ast_node_make(parser->arena, jo_ast_type_literal_string);
			literal_node->data.literal_string = jo_str_view_make(jo_parser_current(parser)->content, jo_parser_current(parser)->content_len);
			break;

		case jo_tok_literal_integer:
			literal_node = jo_ast_node_make(parser->arena, jo_ast_type_literal_i64);
			literal_node->data.literal_i64 = strtoll(jo_parser_current(parser)->content, &end, 10);
			if (errno == ERANGE)
			{
				errno = 0;
				literal_node->type = jo_ast_type_literal_u64;
				literal_node->data.literal_u64 = strtoull(jo_parser_current(parser)->content, &end, 10);
				if (errno == ERANGE)
				{
					printf("integer literal [%.*s] is too big to be contained in 64 bits\n", 
						jo_parser_current(parser)->content_len, 
						jo_parser_current(parser)->content);
					assert(0);
				}
			}
			break;

		case jo_tok_literal_fp:
			literal_node = jo_ast_node_make(parser->arena, jo_ast_type_literal_f64);
			literal_node->data.literal_f64 = strtod(jo_parser_current(parser)->content, &end);
			break;

		case jo_tok_true:
			literal_node = jo_ast_node_make(parser->arena, jo_ast_type_literal_bool);
			literal_node->data.literal_bool = true;
			break;

		case jo_tok_false:
			literal_node = jo_ast_node_make(parser->arena, jo_ast_type_literal_bool);
			literal_node->data.literal_bool = false;
			break;

		default:
			literal_node = jo_ast_node_make(parser->arena, jo_ast_type_literal_type);
			literal_node->data.literal_type.type = jo_parse_type(parser);
			break;
	}

	jo_parser_advance(parser);

	return literal_node;
}

jo_ast_node* jo_parse_literal_fn(jo_parser* parser)
{
	jo_parser_consume(parser, jo_tok_fn);
	jo_ast_node* literal_fn_node = jo_ast_node_make(parser->arena, jo_ast_type_literal_fn);
	jo_ast_literal_fn* literal_fn = &literal_fn_node->data.literal_fn;

	if(jo_parser_current(parser)->type == jo_tok_open_parenthesis)
	{
		jo_parser_consume(parser, jo_tok_open_parenthesis);

		if(jo_parser_current(parser)->type == jo_tok_close_parenthesis)
		{
			jo_parser_consume(parser, jo_tok_close_parenthesis);
		}
		else
		{
			literal_fn->parameters = jo_parse_declaration_list(parser);
			jo_parser_consume(parser, jo_tok_close_parenthesis);
		}
	}

	if(jo_parser_current(parser)->type == jo_tok_arrow)
	{
		jo_parser_consume(parser, jo_tok_arrow);

		literal_fn->return_type = jo_parse_type(parser);
	}
	else
	{
		literal_fn->return_type = jo_ast_node_make(parser->arena, jo_ast_type_type_primitive);
		literal_fn->return_type->data.type_primitive = jo_tok_void;
	}

	if(jo_parser_current(parser)->type == jo_tok_at)
	{
		jo_parser_consume(parser, jo_tok_at);		
		jo_parser_consume(parser, jo_tok_identifier);//@todo: should be specifically "intrinsic"
		literal_fn->intrinsic = true;
	}
	else
	{
		literal_fn->block = jo_parse_block(parser);
	}

	return literal_fn_node;
}

jo_ast_node* jo_parse_expr_assignment(jo_parser* parser)
{
	jo_ast_node* assigment_expr = jo_ast_node_make(parser->arena, jo_ast_type_expr_assignment);
	assigment_expr->data.expr_assignment.target = jo_parse_expression(parser);
	jo_parser_consume(parser, jo_tok_equals);
	assigment_expr->data.expr_assignment.expression = jo_parse_expression(parser);
	return assigment_expr;
}

// jo_ast_node* jo_parse_as_cast(jo_parser* parser)
// {
// 	jo_ast_node* as_cast_node = jo_ast_node_make(parser->arena, jo_ast_expr_as_cast);
// 	as_cast_node->data.expr_as_cast.expr = jo_parse_expression(parser);
// 	jo_parser_consume()
// }

jo_ast_node* jo_parse_literal_struct(jo_parser* parser)
{
	jo_parser_consume(parser, jo_tok_struct);
	jo_ast_node* literal_struct_node = jo_ast_node_make(parser->arena, jo_ast_type_literal_struct); 
	
	jo_parser_consume(parser, jo_tok_open_curly_bracket);
	literal_struct_node->data.literal_struct.members = jo_parse_declaration_list(parser); 
	jo_parser_consume(parser, jo_tok_close_curly_bracket);

	return literal_struct_node;
}

jo_ast_node* jo_parse_op_unary_expression(jo_parser* parser)
{
	// jo_ast_node* node = jo_ast_node_make(parser->arena, jo_parser_current(parser)->type);
	jo_parser_advance(parser);

	// node->data.expr_op_unary.operator_type = jo_tok_minus;
	// node->data.expr_op_unary.expression = jo_parse_expression(parser);
	return NULL;
}

jo_ast_node* jo_parse_primary_expression(jo_parser* parser)
{
	if(jo_tok_is_literal(jo_parser_current(parser)->type)
	|| jo_tok_is_type_primitive(jo_parser_current(parser)->type))
	{
		 return jo_parse_literal_expression(parser);
	}

	switch (jo_parser_current(parser)->type)
	{
	case jo_tok_minus:
		// fall-through
	case jo_tok_exclamation_mark:
		return jo_parse_op_unary_expression(parser);
		break;
	

	case jo_tok_fn:
		return jo_parse_literal_fn(parser);
		break;
	case jo_tok_struct:
		return jo_parse_literal_struct(parser);
		break;
	case jo_tok_identifier:
		if(jo_parser_peek_next(parser)->type == jo_tok_colon
		 || jo_parser_peek_next(parser)->type == jo_tok_walrus
		 || jo_parser_peek_next(parser)->type == jo_tok_bridge)
		{
			return jo_parse_declaration(parser);
		}
		else
		{
			return jo_parse_identifier(parser);
		}
		break;
	case jo_tok_open_parenthesis:
		jo_parser_consume(parser, jo_tok_open_parenthesis);
		jo_ast_node* inner_expr = jo_parse_expression(parser);
		jo_parser_consume(parser, jo_tok_close_parenthesis);
		return inner_expr;
		break;

	default:
		jo_parser_unexpected(parser, "expected expression");
		break;
	}

	return NULL;
}


jo_ast_node* jo_parse_expr_declaration(jo_parser* parser)
{
	jo_ast_node* statement_declaration_node = jo_ast_node_make(parser->arena, jo_ast_type_expr_decl);

	statement_declaration_node->data.expr_decl.declaration = jo_parse_declaration(parser);

	return statement_declaration_node;
}

// jo_ast_node*  jo_parse_expr_for(jo_parser* parser)
// {
// 	jo_parser_consume(parser, jo_tok_for);

// 	jo_ast_node* statement_for_node = jo_ast_node_make(parser->arena, jo_ast_type_expr_for);

// 	statement_for_node->data.expr_for.iterator = jo_parse_expression(parser);

// 	jo_parser_consume(parser, jo_tok_in);

// 	statement_for_node->data.expr_for.iterable = jo_parse_expression(parser);

// 	statement_for_node->data.expr_for.expression = jo_parse_expression(parser);

// 	return statement_for_node;
// }

jo_ast_node* jo_parse_type_fn(jo_parser* parser)
{
	jo_parser_consume(parser, jo_tok_fn);

	jo_ast_node* type_function_node = jo_ast_node_make(parser->arena, jo_ast_type_type_fn);
	jo_ast_type_fn* type_fn = &type_function_node->data.type_fn;

	if(jo_parser_current(parser)->type == jo_tok_open_parenthesis)
	{
		jo_parser_consume(parser, jo_tok_open_parenthesis);

		type_fn->parameters = jo_parse_type_list(parser);

		jo_parser_consume(parser, jo_tok_close_parenthesis);
	}

	if(jo_parser_current(parser)->type == jo_tok_arrow)
	{
		jo_parser_consume(parser, jo_tok_arrow);
		
		type_fn->return_type = jo_parse_type(parser);
	}
	else
	{
		type_fn->return_type = jo_ast_node_make(parser->arena, jo_ast_type_type_primitive);
		type_fn->return_type->data.type_primitive = jo_tok_void;
	}

	return type_function_node;
}

jo_ast_node* jo_parse_identifier(jo_parser* parser)
{
	jo_ast_node* identifier_node = jo_ast_node_make(parser->arena, jo_ast_type_identifier);

	jo_token* t = jo_parser_consume(parser, jo_tok_identifier);

	identifier_node->data.identifier = jo_str_view_make(t->content, t->content_len);

	return identifier_node;
}

jo_ast_node* jo_parse_stmt_return(jo_parser* parser)
{
	jo_parser_consume(parser, jo_tok_return);
	jo_ast_node* stmt_return = jo_ast_node_make(parser->arena, jo_ast_type_stmt_return);
	if(jo_parser_current(parser)->type != jo_tok_close_curly_bracket && jo_parser_current(parser)->type != jo_tok_semicolon)
	{
		stmt_return->data.stmt_return.expression = jo_parse_expression(parser);
	}
	return stmt_return;
}

jo_ast_node* jo_parse_stmt_if(jo_parser* parser)
{
	jo_parser_consume(parser, jo_tok_if);

	jo_ast_node* if_else_node = jo_ast_node_make(parser->arena, jo_ast_type_stmt_ifelse);

	if_else_node->data.stmt_ifelse.condition = jo_parse_expression(parser);
	if_else_node->data.stmt_ifelse.true_block = jo_parse_block(parser);

	if(jo_parser_current(parser)->type == jo_tok_else)
	{
		jo_parser_consume(parser, jo_tok_else);

		if(jo_parser_current(parser)->type == jo_tok_if)
		{
			if_else_node->data.stmt_ifelse.tail_stmt_if = jo_parse_stmt_if(parser);
		}
		else
		{
			if_else_node->data.stmt_ifelse.false_block = jo_parse_block(parser);
		}
	}

	return if_else_node;
}

jo_ast_node*  jo_parse_stmt_expr(jo_parser* parser)
{
	jo_ast_node* expr_stmt = jo_ast_node_make(parser->arena, jo_ast_type_stmt_expr);
	expr_stmt->data.stmt_expr.expr = jo_parse_expression(parser);
	return expr_stmt;
}


jo_ast_node* jo_parse_stmt(jo_parser* parser)
{
	switch (jo_parser_current(parser)->type)
	{
	case jo_tok_if:
		return jo_parse_stmt_if(parser);
		break;

	case jo_tok_return:
		return jo_parse_stmt_return(parser);
		break;

	case jo_tok_open_curly_bracket:
		return jo_parse_block(parser);
		break;

	default:
		return jo_parse_stmt_expr(parser);
		break;
	}
}

jo_ast_node* jo_parse_block(jo_parser* parser)
{
	jo_parser_consume(parser, jo_tok_open_curly_bracket);

	jo_ast_node* block_node = jo_ast_node_make(parser->arena, jo_ast_type_block);

	while(jo_parser_current(parser)->type != jo_tok_close_curly_bracket)
	{
		jo_ada_append(parser->arena, &block_node->data.block.statements, jo_parse_stmt(parser));

		if(jo_parser_current(parser)->type == jo_tok_semicolon)
		{
			jo_parser_consume(parser, jo_tok_semicolon);
		}
	}

	jo_parser_consume(parser, jo_tok_close_curly_bracket);

	return block_node;
}

jo_ast_node* jo_parse_declaration(jo_parser* parser)
{
	jo_ast_node* declaration_node = jo_ast_node_make(parser->arena, jo_ast_type_decl);

	declaration_node->data.decl.identifier = jo_parse_identifier(parser);

	switch (jo_parser_current(parser)->type)
	{
	case jo_tok_colon:
		jo_parser_consume(parser, jo_tok_colon);
		declaration_node->data.decl.specified_type = jo_parse_type(parser);
		if(jo_parser_current(parser)->type == jo_tok_equals)
		{
			jo_parser_consume(parser, jo_tok_equals);
			declaration_node->data.decl.initialize_expression = jo_parse_expression(parser);
		}
		else if(jo_parser_current(parser)->type == jo_tok_colon)
		{
			jo_parser_consume(parser, jo_tok_colon);
			declaration_node->data.decl.initialize_expression = jo_parse_expression(parser);
			declaration_node->data.decl.is_static = true;
		}
		break;
	case jo_tok_walrus:
		jo_parser_consume(parser, jo_tok_walrus);
		declaration_node->data.decl.initialize_expression = jo_parse_expression(parser);
		break;

	case jo_tok_bridge:
		jo_parser_consume(parser, jo_tok_bridge);
		declaration_node->data.decl.initialize_expression = jo_parse_expression(parser);
		declaration_node->data.decl.is_static = true;
		break;

	default:
		break;
	}

	return declaration_node;
}


jo_ast_node_ptr_ada jo_parse_declarations(jo_parser* parser)
{
	jo_ast_node_ptr_ada declaration_nodes = {0};

	while(jo_parser_current(parser)->type != jo_tok_eof)
	{
		jo_ada_append(parser->arena,&declaration_nodes, jo_parse_declaration(parser));
	}

	return declaration_nodes;
}


jo_ast_node* jo_parse_file_content(jo_workspace* workspace, jo_parser* parser)
{
	switch (jo_parser_current(parser)->type)
	{
	case jo_tok_at:
		jo_parser_advance(parser);
		if(jo_parser_current(parser)->type == jo_tok_identifier)
		{
			if(strncmp(jo_parser_current(parser)->content, "load", jo_parser_current(parser)->content_len) == 0)
			{
				jo_parser_advance(parser);
				jo_parser_consume(parser, jo_tok_open_parenthesis);		
				
				jo_astr path = jo_astr_clone(&workspace->arena, &workspace->current_directory);
				jo_astr_append(&workspace->arena, &path, "\\");
				jo_astr_append_view(&workspace->arena, &path, jo_str_view_make(jo_parser_current(parser)->content, jo_parser_current(parser)->content_len));		

				// @TODO: normalzing path on spot might be a good idea		
				jo_ada_append(
					&workspace->arena, 
					workspace->load_queue, 
					path
				);
				
				jo_parser_consume(parser, jo_tok_literal_string);				
				jo_parser_consume(parser, jo_tok_close_parenthesis);				
			}
		}
		return NULL;
		break;

	default:
		return jo_parse_declaration(parser);
		break;
	}
}

jo_ast_node* jo_parse_file(jo_workspace* workspace, jo_parser* parser)
{
	jo_ast_node* module = jo_ast_node_make(parser->arena, jo_ast_type_file);

	while(jo_parser_current(parser)->type != jo_tok_eof)
	{
		jo_ast_node* node = jo_parse_file_content(workspace, parser);
		if(node)
		{
			jo_ada_append(parser->arena, &module->data.file.content, node);
		}
	}

	return module;
}

jo_ast_node* jo_parse(jo_workspace* workspace, jo_parser* parser)
{	
	return jo_parse_file(workspace, parser);
}
