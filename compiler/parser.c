#include "parser.h"
#include "token.h"
#include "workspace.h"
#include <errno.h>
#include <stdio.h>

token* parser_peek(parser* parser, u32 offset)
{
	if(parser->current_token + offset < parser->tokens->occupied)
	{
		return parser->tokens->data + parser->current_token + offset;
	}

	assert(0 && "unexpected end %s");
}

token* parser_peek_next(parser* parser)
{
	return parser_peek(parser, 1);
}

void parser_advance(parser* parser)
{
	parser->current_token++;
}

token* parser_current(parser* parser)
{
	// dump_tokens_2(parser->lexer);
	return parser->tokens->data + parser->current_token;
}

void parser_unexpected(parser* parser, const char* err)
{	
	printf("unexpected: %s at line %d column %d got %s", err, parser_current(parser)->line, parser_current(parser)->column, tok_to_string(parser_current(parser)->type));
	assert(0);
}

token* parser_consume(parser* parser, tok expected)
{
    token* current = parser_current(parser);
    if(current->type != expected)
    {
        printf("unexpected token %s, expected %s at line %d column %d",
            tok_to_string(current->type),
            tok_to_string(expected),
            current->line,
			current->column);

		assert(0);
    }

	parser_advance(parser);
	return current;
}

ast_node_ptr_ada parse_type_function_parameters(parser* parser)
{
	ast_node_ptr_ada parameter_nodes = {0};

	parser_consume(parser, tok_open_parenthesis);


	while(true)
	{
		if(parser_current(parser)->type == tok_minus) // minus from return separator arrow (->)
		{
			break;
		}

		if(parser_current(parser)->type ==  tok_close_parenthesis) // implicid -> void
		{
			break;
		}

		if(parameter_nodes.occupied > 0)
		{
			parser_consume(parser, tok_comma);
		}

		ada_append(parser->arena,
			&parameter_nodes,
			parse_declaration(parser)
		);
	}


	return parameter_nodes;
}

ast_node* parse_type_primitive(parser* parser)
{
	ast_node* type_primitive_node = ast_node_make(parser->arena, ast_type_type_primitive);
	type_primitive_node->data.type_primitive = parser_current(parser)->type;
	parser_advance(parser);
	return type_primitive_node;
}

ast_node* parse_type(parser* parser)
{
	if(tok_is_type_primitive(parser_current(parser)->type)){ return parse_type_primitive(parser); }

	ast_node* type_node = {0};
	switch (parser_current(parser)->type)
	{
	case tok_open_square_bracket:
		parser_consume(parser, tok_open_square_bracket);
		type_node = ast_node_make(parser->arena, ast_type_type_array);
		if(parser_current(parser)->type != tok_close_square_bracket)
		{
			type_node->data.type_array.array_size_expression = parse_expression(parser);
		}
		parser_consume(parser, tok_close_square_bracket);
		type_node->data.type_ptr.inner = parse_type(parser);
		break;

	case tok_star:
		parser_consume(parser, tok_star);
		type_node = ast_node_make(parser->arena, ast_type_type_ptr);
		type_node->data.type_ptr.inner = parse_type(parser);
		break;

	case tok_ampersand:
		parser_consume(parser, tok_ampersand);
		type_node = ast_node_make(parser->arena, ast_type_type_ref);
		type_node->data.type_ref.inner = parse_type(parser);
		break;

	case tok_identifier:
		type_node = parse_identifier(parser);
		break;

	case tok_fn:
		type_node = parse_type_fn(parser);
		break;
	default:
		assert(0);
		break;
	}
	//------------------------------------------------------------------

	if(!type_node)
	{
		parser_unexpected(parser, "expected type");
	}

	return type_node;
}

ast_node_ptr_ada parse_expression_list(parser* parser)
{
	ast_node_ptr_ada expression_list = {0};
	while(true)
	{
		if(expression_list.occupied > 0)
		{
			if(parser_current(parser)->type == tok_comma)
			{
				parser_consume(parser, tok_comma);
			}
			else
			{
				return expression_list;
			}
		}

		ada_append(parser->arena,&expression_list, parse_expression(parser));
	}
}

ast_node_ptr_ada parse_type_list(parser* parser)
{
	ast_node_ptr_ada type_list = {0};

	while(true)
	{
		if(type_list.occupied > 0)
		{
			if(parser_current(parser)->type == tok_comma)
			{
				parser_consume(parser, tok_comma);
			}
			else
			{
				return type_list;
			}
		}

		ada_append(parser->arena,&type_list, parse_type(parser));
	}
}

ast_node_ptr_ada parse_declaration_list(parser* parser)
{
	ast_node_ptr_ada declaration_list = {0};

	while(true)
	{
		if(declaration_list.occupied > 0)
		{
			if(parser_current(parser)->type == tok_comma)
			{
				parser_consume(parser, tok_comma);
			}
			else
			{
				return declaration_list;
			}
		}

		ada_append(parser->arena,&declaration_list, parse_declaration(parser));
	}
}

ast_node* try_parse_expression_postfix_operator(parser* parser, ast_node* expression)
{
	switch (parser_current(parser)->type)
	{
	case tok_open_parenthesis:
		parser_consume(parser, tok_open_parenthesis);
		ast_node* apply_paren_operation_node = ast_node_make(parser->arena, ast_type_expr_op_call);
		apply_paren_operation_node->data.expr_op_call.target = expression;
		if(parser_current(parser)->type != tok_close_parenthesis)
		{
			apply_paren_operation_node->data.expr_op_call.arguments = parse_expression_list(parser);
		}
		parser_consume(parser, tok_close_parenthesis);
		return apply_paren_operation_node;
		break;

	case tok_open_square_bracket:
		parser_consume(parser, tok_open_square_bracket);
		ast_node* apply_squaer_brackets_operation_node = ast_node_make(parser->arena, ast_type_expr_op_index);
		apply_squaer_brackets_operation_node->data.expr_op_index.target = expression;
		if(parser_current(parser)->type != tok_close_square_bracket)
		{
			apply_squaer_brackets_operation_node->data.expr_op_index.arguments = parse_expression_list(parser);
		}
		parser_consume(parser, tok_close_square_bracket);
		return apply_squaer_brackets_operation_node;
		break;

	case tok_as:
		parser_consume(parser, tok_as);
		ast_node* as_cast_node = ast_node_make(parser->arena, ast_type_expr_as_cast);
		as_cast_node->data.expr_as_cast.to_type = parse_type(parser);
		as_cast_node->data.expr_as_cast.expr = expression;
		return as_cast_node;
		break;

	default:
		return NULL;
		break;
	}
}

ast_node* parse_expression_precedented(parser* parser, u32 min_precedence)
{
	ast_node* left_expression_node = parse_primary_expression(parser);

	while(true)
	{
		ast_node* postfix_expression = NULL;

		do{
			postfix_expression = try_parse_expression_postfix_operator(parser, left_expression_node);
			if(postfix_expression) { left_expression_node = postfix_expression; }
		}while(postfix_expression);

		tok operator_type = parser_current(parser)->type;
		if(!tok_is_operator(operator_type)) break; // not an operator, statemt end

		u32 precedence = tok_binary_operator_precedence(operator_type);
		if(precedence < min_precedence)
		{
			break;
		}

		parser_advance(parser);

		ast_node* right_expression_node = parse_expression_precedented(parser, precedence + 1);

		ast_node* new_left_expression_node = ast_node_make(parser->arena, ast_type_expr_op_binary);

		new_left_expression_node->data.expr_op_binary.operator_type = operator_type;
		new_left_expression_node->data.expr_op_binary.left_expression = left_expression_node;
		new_left_expression_node->data.expr_op_binary.right_expression = right_expression_node;

		left_expression_node = new_left_expression_node;
	}


	return left_expression_node;
}


ast_node* parse_expression(parser* parser)
{
	return parse_expression_precedented(parser, 0);
}

ast_node* parse_literal_expression(parser* parser)
{
	ast_node* literal_node = NULL;
	char* end;

	switch (parser_current(parser)->type)
	{
		case tok_literal_string:
			literal_node = ast_node_make(parser->arena, ast_type_literal_string);
			literal_node->data.literal_string = str_view_make(parser_current(parser)->content, parser_current(parser)->content_len);
			break;

		case tok_literal_integer:
			literal_node = ast_node_make(parser->arena, ast_type_literal_i64);
			literal_node->data.literal_i64 = strtoll(parser_current(parser)->content, &end, 10);
			if (errno == ERANGE)
			{
				errno = 0;
				literal_node->type = ast_type_literal_u64;
				literal_node->data.literal_u64 = strtoull(parser_current(parser)->content, &end, 10);
				if (errno == ERANGE)
				{
					printf("integer literal [%.*s] is too big to be contained in 64 bits\n", 
						parser_current(parser)->content_len, 
						parser_current(parser)->content);
					assert(0);
				}
			}
			break;

		case tok_literal_fp:
			literal_node = ast_node_make(parser->arena, ast_type_literal_f64);
			literal_node->data.literal_f64 = strtod(parser_current(parser)->content, &end);
			break;

		case tok_true:
			literal_node = ast_node_make(parser->arena, ast_type_literal_bool);
			literal_node->data.literal_bool = true;
			break;

		case tok_false:
			literal_node = ast_node_make(parser->arena, ast_type_literal_bool);
			literal_node->data.literal_bool = false;
			break;

		default:
			literal_node = ast_node_make(parser->arena, ast_type_literal_type);
			literal_node->data.literal_type.type = parse_type(parser);
			break;
	}

	parser_advance(parser);

	return literal_node;
}

ast_node* parse_literal_fn(parser* parser)
{
	parser_consume(parser, tok_fn);
	ast_node* literal_fn_node = ast_node_make(parser->arena, ast_type_literal_fn);
	ast_literal_fn* literal_fn = &literal_fn_node->data.literal_fn;

	if(parser_current(parser)->type == tok_open_parenthesis)
	{
		parser_consume(parser, tok_open_parenthesis);

		if(parser_current(parser)->type == tok_close_parenthesis)
		{
			parser_consume(parser, tok_close_parenthesis);
		}
		else
		{
			literal_fn->parameters = parse_declaration_list(parser);
			parser_consume(parser, tok_close_parenthesis);
		}
	}

	if(parser_current(parser)->type == tok_arrow)
	{
		parser_consume(parser, tok_arrow);

		literal_fn->return_type = parse_type(parser);
	}
	else
	{
		literal_fn->return_type = ast_node_make(parser->arena, ast_type_type_primitive);
		literal_fn->return_type->data.type_primitive = tok_void;
	}

	if(parser_current(parser)->type == tok_at)
	{
		parser_consume(parser, tok_at);		
		parser_consume(parser, tok_identifier);//@todo: should be specifically "intrinsic"
		literal_fn->intrinsic = true;
	}
	else
	{
		literal_fn->block = parse_block(parser);
	}

	return literal_fn_node;
}

ast_node* parse_expr_assignment(parser* parser)
{
	ast_node* assigment_expr = ast_node_make(parser->arena, ast_type_expr_assignment);
	assigment_expr->data.expr_assignment.target = parse_expression(parser);
	parser_consume(parser, tok_equals);
	assigment_expr->data.expr_assignment.expression = parse_expression(parser);
	return assigment_expr;
}

// ast_node* parse_as_cast(parser* parser)
// {
// 	ast_node* as_cast_node = ast_node_make(parser->arena, ast_expr_as_cast);
// 	as_cast_node->data.expr_as_cast.expr = parse_expression(parser);
// 	parser_consume()
// }

ast_node* parse_literal_struct(parser* parser)
{
	parser_consume(parser, tok_struct);
	ast_node* literal_struct_node = ast_node_make(parser->arena, ast_type_literal_struct); 
	
	parser_consume(parser, tok_open_curly_bracket);
	literal_struct_node->data.literal_struct.members = parse_declaration_list(parser); 
	parser_consume(parser, tok_close_curly_bracket);

	return literal_struct_node;
}

ast_node* parse_op_unary_expression(parser* parser)
{
	// ast_node* node = ast_node_make(parser->arena, parser_current(parser)->type);
	parser_advance(parser);

	// node->data.expr_op_unary.operator_type = tok_minus;
	// node->data.expr_op_unary.expression = parse_expression(parser);
	return NULL;
}

ast_node* parse_primary_expression(parser* parser)
{
	if(tok_is_literal(parser_current(parser)->type)
	|| tok_is_type_primitive(parser_current(parser)->type))
	{
		 return parse_literal_expression(parser);
	}

	switch (parser_current(parser)->type)
	{
	case tok_minus:
		// fall-through
	case tok_exclamation_mark:
		return parse_op_unary_expression(parser);
		break;
	

	case tok_fn:
		return parse_literal_fn(parser);
		break;
	case tok_struct:
		return parse_literal_struct(parser);
		break;
	case tok_identifier:
		if(parser_peek_next(parser)->type == tok_colon
		 || parser_peek_next(parser)->type == tok_walrus
		 || parser_peek_next(parser)->type == tok_bridge)
		{
			return parse_declaration(parser);
		}
		else
		{
			return parse_identifier(parser);
		}
		break;
	case tok_open_parenthesis:
		parser_consume(parser, tok_open_parenthesis);
		ast_node* inner_expr = parse_expression(parser);
		parser_consume(parser, tok_close_parenthesis);
		return inner_expr;
		break;

	default:
		parser_unexpected(parser, "expected expression");
		break;
	}

	return NULL;
}


ast_node* parse_expr_declaration(parser* parser)
{
	ast_node* statement_declaration_node = ast_node_make(parser->arena, ast_type_expr_decl);

	statement_declaration_node->data.expr_decl.declaration = parse_declaration(parser);

	return statement_declaration_node;
}

// ast_node*  parse_expr_for(parser* parser)
// {
// 	parser_consume(parser, tok_for);

// 	ast_node* statement_for_node = ast_node_make(parser->arena, ast_type_expr_for);

// 	statement_for_node->data.expr_for.iterator = parse_expression(parser);

// 	parser_consume(parser, tok_in);

// 	statement_for_node->data.expr_for.iterable = parse_expression(parser);

// 	statement_for_node->data.expr_for.expression = parse_expression(parser);

// 	return statement_for_node;
// }

ast_node* parse_type_fn(parser* parser)
{
	parser_consume(parser, tok_fn);

	ast_node* type_function_node = ast_node_make(parser->arena, ast_type_type_fn);
	ast_type_fn* type_fn = &type_function_node->data.type_fn;

	if(parser_current(parser)->type == tok_open_parenthesis)
	{
		parser_consume(parser, tok_open_parenthesis);

		type_fn->parameters = parse_type_list(parser);

		parser_consume(parser, tok_close_parenthesis);
	}

	if(parser_current(parser)->type == tok_arrow)
	{
		parser_consume(parser, tok_arrow);
		
		type_fn->return_type = parse_type(parser);
	}
	else
	{
		type_fn->return_type = ast_node_make(parser->arena, ast_type_type_primitive);
		type_fn->return_type->data.type_primitive = tok_void;
	}

	return type_function_node;
}

ast_node* parse_identifier(parser* parser)
{
	ast_node* identifier_node = ast_node_make(parser->arena, ast_type_identifier);

	token* t = parser_consume(parser, tok_identifier);

	identifier_node->data.identifier = str_view_make(t->content, t->content_len);

	return identifier_node;
}

ast_node* parse_stmt_return(parser* parser)
{
	parser_consume(parser, tok_return);
	ast_node* stmt_return = ast_node_make(parser->arena, ast_type_stmt_return);
	if(parser_current(parser)->type != tok_close_curly_bracket && parser_current(parser)->type != tok_semicolon)
	{
		stmt_return->data.stmt_return.expression = parse_expression(parser);
	}
	return stmt_return;
}

ast_node* parse_stmt_if(parser* parser)
{
	parser_consume(parser, tok_if);

	ast_node* if_else_node = ast_node_make(parser->arena, ast_type_stmt_ifelse);

	if_else_node->data.stmt_ifelse.condition = parse_expression(parser);
	if_else_node->data.stmt_ifelse.true_block = parse_block(parser);

	if(parser_current(parser)->type == tok_else)
	{
		parser_consume(parser, tok_else);

		if(parser_current(parser)->type == tok_if)
		{
			if_else_node->data.stmt_ifelse.tail_stmt_if = parse_stmt_if(parser);
		}
		else
		{
			if_else_node->data.stmt_ifelse.false_block = parse_block(parser);
		}
	}

	return if_else_node;
}

ast_node*  parse_stmt_expr(parser* parser)
{
	ast_node* expr_stmt = ast_node_make(parser->arena, ast_type_stmt_expr);
	expr_stmt->data.stmt_expr.expr = parse_expression(parser);
	return expr_stmt;
}


ast_node* parse_stmt(parser* parser)
{
	switch (parser_current(parser)->type)
	{
	case tok_if:
		return parse_stmt_if(parser);
		break;

	case tok_return:
		return parse_stmt_return(parser);
		break;

	case tok_open_curly_bracket:
		return parse_block(parser);
		break;

	default:
		return parse_stmt_expr(parser);
		break;
	}
}

ast_node* parse_block(parser* parser)
{
	parser_consume(parser, tok_open_curly_bracket);

	ast_node* block_node = ast_node_make(parser->arena, ast_type_block);

	while(parser_current(parser)->type != tok_close_curly_bracket)
	{
		ada_append(parser->arena, &block_node->data.block.statements, parse_stmt(parser));

		if(parser_current(parser)->type == tok_semicolon)
		{
			parser_consume(parser, tok_semicolon);
		}
	}

	parser_consume(parser, tok_close_curly_bracket);

	return block_node;
}

ast_node* parse_declaration(parser* parser)
{
	ast_node* declaration_node = ast_node_make(parser->arena, ast_type_decl);

	declaration_node->data.decl.identifier = parse_identifier(parser);

	switch (parser_current(parser)->type)
	{
	case tok_colon:
		parser_consume(parser, tok_colon);
		declaration_node->data.decl.specified_type = parse_type(parser);
		if(parser_current(parser)->type == tok_equals)
		{
			parser_consume(parser, tok_equals);
			declaration_node->data.decl.initialize_expression = parse_expression(parser);
		}
		else if(parser_current(parser)->type == tok_colon)
		{
			parser_consume(parser, tok_colon);
			declaration_node->data.decl.initialize_expression = parse_expression(parser);
			declaration_node->data.decl.is_static = true;
		}
		break;
	case tok_walrus:
		parser_consume(parser, tok_walrus);
		declaration_node->data.decl.initialize_expression = parse_expression(parser);
		break;

	case tok_bridge:
		parser_consume(parser, tok_bridge);
		declaration_node->data.decl.initialize_expression = parse_expression(parser);
		declaration_node->data.decl.is_static = true;
		break;

	default:
		break;
	}

	return declaration_node;
}


ast_node_ptr_ada parse_declarations(parser* parser)
{
	ast_node_ptr_ada declaration_nodes = {0};

	while(parser_current(parser)->type != tok_eof)
	{
		ada_append(parser->arena,&declaration_nodes, parse_declaration(parser));
	}

	return declaration_nodes;
}


ast_node* parse_file_content(workspace* workspace, parser* parser)
{
	switch (parser_current(parser)->type)
	{
	case tok_at:
		parser_advance(parser);
		if(parser_current(parser)->type == tok_identifier)
		{
			if(strncmp(parser_current(parser)->content, "load", parser_current(parser)->content_len) == 0)
			{
				parser_advance(parser);
				
				astr path = astr_clone(&workspace->arena, &workspace->current_directory);
				astr_append(&workspace->arena, &path, "\\");
				astr_append_view(&workspace->arena, &path, str_view_make(parser_current(parser)->content, parser_current(parser)->content_len));		

				// @TODO: normalzing path on spot might be a good idea		
				ada_append(
					&workspace->arena, 
					workspace->load_queue, 
					path
				);
				
				parser_consume(parser, tok_literal_string);				
			}
		}
		return NULL;
		break;

	default:
		return parse_declaration(parser);
		break;
	}
}

ast_node* parse_file(workspace* workspace, parser* parser)
{
	ast_node* module = ast_node_make(parser->arena, ast_type_file);

	while(parser_current(parser)->type != tok_eof)
	{
		ast_node* node = parse_file_content(workspace, parser);

		if(node)
		{
			ada_append(parser->arena, &module->data.file.content, node);
		}
	}

	return module;
}

ast_node* parse(workspace* workspace, parser* parser)
{	
	return parse_file(workspace, parser);
}
