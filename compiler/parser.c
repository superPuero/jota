#include "parser.h"
#include "token.h"
#include <errno.h>
#include <stdio.h>

jo_token_t* jo_parser_peek(jo_parser_t* parser, jo_u32 offset)
{
	if(parser->current_token + offset < parser->lexer->tokens.occupied)
	{
		return parser->lexer->tokens.data + parser->current_token + offset;
	}

	jo_err("unexpected end %s", "");
}

jo_token_t* jo_parser_peek_next(jo_parser_t* parser)
{
	return jo_parser_peek(parser, 1);
}

void jo_parser_advance(jo_parser_t* parser)
{
	parser->current_token++;
}

jo_token_t* jo_parser_current(jo_parser_t* parser)
{
	return parser->lexer->tokens.data + parser->current_token;
}


void jo_parser_unexpected(jo_parser_t* parser, const char* err)
{
	jo_err("unexpected: %s at line %d column %d got %s", err, jo_parser_current(parser)->line, jo_parser_current(parser)->column, jo_token_type_to_string(jo_parser_current(parser)->type));
}

jo_ast_node_t* jo_make_ast_node(jo_ast_node_type_t type)
{
	jo_ast_node_t* node = calloc(sizeof(jo_ast_node_t), 1);
	node->type = type;
	return node;
}


jo_token_t* jo_parser_consume(jo_parser_t* parser, jo_token_type_t expected)
{
    jo_token_t* current = jo_parser_current(parser);
    if(current->type != expected)
    {
        jo_err("unexpected token %s, expected %s at line %d column %d",
            jo_token_type_to_string(current->type),
            jo_token_type_to_string(expected),
            current->line,
			current->column);
    }

	jo_parser_advance(parser);
	return current;
}

jo_ast_node_ptr_dyn_array_t jo_parse_type_function_parameters(jo_parser_t* parser)
{
	jo_ast_node_ptr_dyn_array_t parameter_nodes = {0};

	jo_parser_consume(parser, jo_token_open_parenthesis);

	while(true)
	{
		if(jo_parser_current(parser)->type == jo_token_minus) // minus from return separator arrow (->)
		{
			break;
		}

		if(jo_parser_current(parser)->type ==  jo_token_close_parenthesis) // implicid -> void
		{
			break;
		}

		if(parameter_nodes.occupied > 0)
		{
			jo_parser_consume(parser, jo_token_comma);
		}

		jo_dyn_array_append(
			&parameter_nodes,
			jo_parse_declaration(parser)
		);
	}


	return parameter_nodes;
}

jo_ast_node_t* jo_parse_type_primitive(jo_parser_t* parser)
{
	jo_ast_node_t* type_primitive_node = jo_make_ast_node(jo_ast_type_type_primitive);
	type_primitive_node->data.type_primitive = jo_parser_current(parser)->type;
	jo_parser_advance(parser);
	return type_primitive_node;
}

jo_ast_node_ptr_dyn_array_t jo_parse_type_extent(jo_parser_t* parser)
{
	return (jo_ast_node_ptr_dyn_array_t){0};
}

jo_ast_node_t* jo_parse_type(jo_parser_t* parser)
{
	if(jo_parser_current(parser)->kind == jo_token_kind_type_primitive){ return jo_parse_type_primitive(parser); }

	jo_ast_node_t* type_node = {0};
	switch (jo_parser_current(parser)->type)
	{
	case jo_token_open_square_bracket:
		jo_parser_consume(parser, jo_token_open_square_bracket);
		type_node = jo_make_ast_node(jo_ast_type_type_array);
		if(jo_parser_current(parser)->type != jo_token_close_square_bracket)
		{
			type_node->data.type_array.array_size_expression = jo_parse_expression(parser);
		}
		jo_parser_consume(parser, jo_token_close_square_bracket);
		type_node->data.type_ptr.inner = jo_parse_type(parser);
		break;

	case jo_token_star:
		jo_parser_consume(parser, jo_token_star);
		type_node = jo_make_ast_node(jo_ast_type_type_ptr);
		type_node->data.type_ptr.inner = jo_parse_type(parser);
		break;

	case jo_token_ampersand:
		jo_parser_consume(parser, jo_token_ampersand);
		type_node = jo_make_ast_node(jo_ast_type_type_ref);
		type_node->data.type_ref.inner = jo_parse_type(parser);
		break;

	case jo_token_identifier:
		type_node = jo_parse_identifier(parser);
		break;

	case jo_token_keyword_fn:
		type_node = jo_parse_type_fn(parser);
		break;
	}
	//------------------------------------------------------------------

	if(!type_node)
	{
		jo_parser_unexpected(parser, "expected type");
	}

	return type_node;
}

jo_ast_node_ptr_dyn_array_t jo_parse_expression_list(jo_parser_t* parser)
{
	jo_ast_node_ptr_dyn_array_t expression_list = {0};
	while(true)
	{
		if(expression_list.occupied > 0)
		{
			if(jo_parser_current(parser)->type == jo_token_comma)
			{
				jo_parser_consume(parser, jo_token_comma);
			}
			else
			{
				return expression_list;
			}
		}

		jo_dyn_array_append(&expression_list, jo_parse_expression(parser));
	}
}

jo_ast_node_ptr_dyn_array_t jo_parse_type_list(jo_parser_t* parser)
{
	jo_ast_node_ptr_dyn_array_t type_list = {0};

	while(true)
	{
		if(type_list.occupied > 0)
		{
			if(jo_parser_current(parser)->type == jo_token_comma)
			{
				jo_parser_consume(parser, jo_token_comma);
			}
			else
			{
				return type_list;
			}
		}

		jo_dyn_array_append(&type_list, jo_parse_type(parser));
	}
}

jo_ast_node_ptr_dyn_array_t jo_parse_declaration_list(jo_parser_t* parser)
{
	jo_ast_node_ptr_dyn_array_t declaration_list = {0};

	while(true)
	{
		if(declaration_list.occupied > 0)
		{
			if(jo_parser_current(parser)->type == jo_token_comma)
			{
				jo_parser_consume(parser, jo_token_comma);
			}
			else
			{
				return declaration_list;
			}
		}

		jo_dyn_array_append(&declaration_list, jo_parse_declaration(parser));
	}
}

jo_ast_node_t* jo_try_parse_expression_postfix_operator(jo_parser_t* parser, jo_ast_node_t* expression)
{
	switch (jo_parser_current(parser)->type)
	{
	case jo_token_open_parenthesis:
		jo_parser_consume(parser, jo_token_open_parenthesis);
		jo_ast_node_t* apply_paren_operation_node = jo_make_ast_node(jo_ast_type_expr_op_call);
		apply_paren_operation_node->data.expr_op_call.target = expression;
		if(jo_parser_current(parser)->type != jo_token_close_parenthesis)
		{
			apply_paren_operation_node->data.expr_op_call.arguments = jo_parse_expression_list(parser);
		}
		jo_parser_consume(parser, jo_token_close_parenthesis);
		return apply_paren_operation_node;
		break;

	case jo_token_open_square_bracket:
		jo_parser_consume(parser, jo_token_open_square_bracket);
		jo_ast_node_t* apply_squaer_brackets_operation_node = jo_make_ast_node(jo_ast_type_expr_op_index);
		apply_squaer_brackets_operation_node->data.expr_op_index.target = expression;
		if(jo_parser_current(parser)->type != jo_token_close_square_bracket)
		{
			apply_squaer_brackets_operation_node->data.expr_op_index.arguments = jo_parse_expression_list(parser);
		}
		jo_parser_consume(parser, jo_token_close_square_bracket);
		return apply_squaer_brackets_operation_node;
		break;

	case jo_token_keyword_as:
		jo_parser_consume(parser, jo_token_keyword_as);
		jo_ast_node_t* as_cast_node = jo_make_ast_node(jo_ast_type_expr_as_cast);
		as_cast_node->data.expr_as_cast.to_type = jo_parse_type(parser);
		as_cast_node->data.expr_as_cast.expr = expression;
		return as_cast_node;
		break;

	default:
		return NULL;
		break;
	}
}

jo_ast_node_t* jo_parse_expression_precedented(jo_parser_t* parser, jo_u32 min_precedence)
{
	jo_ast_node_t* left_expression_node = jo_parse_primary_expression(parser);

	while(true)
	{
		jo_ast_node_t* postfix_expression = NULL;

		do{
			postfix_expression = jo_try_parse_expression_postfix_operator(parser, left_expression_node);
			if(postfix_expression) { left_expression_node = postfix_expression; }
		}while(postfix_expression);

		jo_token_type_t operator_type = jo_parser_current(parser)->type;
		jo_u32 precedence = jo_token_binary_operator_precedence(operator_type);

		if(precedence == 0) break; // not an operator, statemt end

		if(precedence < min_precedence)
		{
			break;
		}

		jo_parser_advance(parser);

		jo_ast_node_t* right_expression_node = jo_parse_expression_precedented(parser, precedence + 1);

		jo_ast_node_t* new_left_expression_node = jo_make_ast_node(jo_ast_type_expr_op_binary);

		new_left_expression_node->data.expr_op_binary.operator_type = operator_type;
		new_left_expression_node->data.expr_op_binary.left_expression = left_expression_node;
		new_left_expression_node->data.expr_op_binary.right_expression = right_expression_node;

		left_expression_node = new_left_expression_node;
	}


	return left_expression_node;
}


jo_ast_node_t* jo_parse_expression(jo_parser_t* parser)
{
	return jo_parse_expression_precedented(parser, 0);
}

jo_ast_node_t* jo_parse_literal_expression(jo_parser_t* parser)
{
	jo_ast_node_t* literal_node = NULL;
	char* end;

	switch (jo_parser_current(parser)->type)
	{
		case jo_token_literal_string:
			literal_node = jo_make_ast_node(jo_ast_type_literal_string);
			literal_node->data.literal_string = jo_string_from_n(jo_parser_current(parser)->content, jo_parser_current(parser)->content_len);
			break;
		case jo_token_literal_u64:
			literal_node = jo_make_ast_node(jo_ast_type_literal_u64);
			literal_node->data.literal_u64 = strtoull(jo_parser_current(parser)->content, &end, 10);
			break;

		case jo_token_literal_i64:
			literal_node = jo_make_ast_node(jo_ast_type_literal_i64);
			literal_node->data.literal_i64 = strtoll(jo_parser_current(parser)->content, &end, 10);
			break;

		case jo_token_literal_u32:
			literal_node = jo_make_ast_node(jo_ast_type_literal_u32);
			literal_node->data.literal_u32 = strtoul(jo_parser_current(parser)->content, &end, 10);
			break;
		case jo_token_literal_i32:
			literal_node = jo_make_ast_node(jo_ast_type_literal_i32);
			literal_node->data.literal_i32 = strtol(jo_parser_current(parser)->content, &end, 10);
			break;

		case jo_token_literal_u16:
		{
			literal_node = jo_make_ast_node(jo_ast_type_literal_u16);
			jo_u32 valueU16 = strtoul(jo_parser_current(parser)->content, &end, 10);

			if (jo_parser_current(parser)->content == end) return false;
		    if (*end != '\0') return false;

			if (errno == ERANGE || valueU16 > UINT16_MAX)
			{
				jo_err("invalid u16%s", "\n");
			}

			literal_node->data.literal_u16 = (jo_u16)valueU16;
			break;
		}

		case jo_token_literal_i16:
		{
			literal_node = jo_make_ast_node(jo_ast_type_literal_i16);
			jo_i32 valueI16 = strtol(jo_parser_current(parser)->content, &end, 10);

			if (jo_parser_current(parser)->content == end) return false;
		    if (*end != '\0') return false;

			if (errno == ERANGE || valueI16 < INT16_MIN || valueI16 > INT16_MAX)
			{
				jo_err("invalid i16%s", "\n");
			}

			literal_node->data.literal_i16 = (jo_i16)valueI16;
			break;
		}

		case jo_token_literal_u8:
		{
			literal_node = jo_make_ast_node(jo_ast_type_literal_u8);
			jo_u32 valueU8 = strtoul(jo_parser_current(parser)->content, &end, 10);

			if (jo_parser_current(parser)->content == end) return false;
		    if (*end != '\0') return false;

			if (errno == ERANGE || valueU8 > UINT8_MAX)
			{
				jo_err("invalid u8%s", "\n");
			}

			literal_node->data.literal_u8 = (jo_u8)valueU8;
			break;
		}

		case jo_token_literal_i8:
		{
			literal_node = jo_make_ast_node(jo_ast_type_literal_i8);
			jo_i32 valueI8 = strtol(jo_parser_current(parser)->content, &end, 10);

			if (jo_parser_current(parser)->content == end) return false;
		    if (*end != '\0') return false;

			if (errno == ERANGE || valueI8 < INT8_MIN || valueI8 > INT8_MAX)
			{
				printf("%d < %d: %d\n", valueI8, INT8_MIN, valueI8 < INT8_MIN);
				jo_err("invalid i8%s", "\n");
			}

			literal_node->data.literal_i8 = (jo_i8)valueI8;
			break;
		}

		case jo_token_literal_f32:
			literal_node = jo_make_ast_node(jo_ast_type_literal_f32);
			literal_node->data.literal_f32 = strtof(jo_parser_current(parser)->content, &end);
			break;

		case jo_token_literal_f64:
			literal_node = jo_make_ast_node(jo_ast_type_literal_f64);
			literal_node->data.literal_f64 = strtod(jo_parser_current(parser)->content, &end);
			break;

		case jo_token_keyword_true:
			literal_node = jo_make_ast_node(jo_ast_type_literal_bool);
			literal_node->data.literal_bool = true;
			break;

		case jo_token_keyword_false:
			literal_node = jo_make_ast_node(jo_ast_type_literal_bool);
			literal_node->data.literal_bool = false;
			break;

		default:
			literal_node = jo_make_ast_node(jo_ast_type_literal_type);
			literal_node->data.literal_type.type = jo_parse_type(parser);
			break;
	}

	jo_parser_advance(parser);

	return literal_node;
}

jo_ast_node_t* jo_parse_literal_fn(jo_parser_t* parser)
{
	jo_parser_consume(parser, jo_token_keyword_fn);
	jo_ast_node_t* literal_fn_node = jo_make_ast_node(jo_ast_type_literal_fn);
	jo_ast_literal_fn* literal_fn = &literal_fn_node->data.literal_fn;

	if(jo_parser_current(parser)->type == jo_token_open_parenthesis)
	{
		jo_parser_consume(parser, jo_token_open_parenthesis);

		literal_fn->parameters = jo_parse_declaration_list(parser);

		jo_parser_consume(parser, jo_token_close_parenthesis);
	}

	if(jo_parser_current(parser)->type == jo_token_arrow)
	{
		jo_parser_consume(parser, jo_token_arrow);

		literal_fn->return_type = jo_parse_type(parser);
	}
	else
	{
		literal_fn->return_type = jo_make_ast_node(jo_ast_type_type_primitive);
		literal_fn->return_type->data.type_primitive = jo_token_keyword_void;
	}

	if(jo_parser_current(parser)->type == jo_token_keyword_intrinsic)
	{
		jo_parser_consume(parser, jo_token_keyword_intrinsic);
		literal_fn->intrinsic = true;
	}
	else
	{
		literal_fn->block = jo_parse_block(parser);
	}

	return literal_fn_node;
}

jo_ast_node_t* jo_parse_expr_assignment(jo_parser_t* parser)
{
	jo_ast_node_t* assigment_expr = jo_make_ast_node(jo_ast_type_expr_assignment);
	assigment_expr->data.expr_assignment.target = jo_parse_expression(parser);
	jo_parser_consume(parser, jo_token_equals);
	assigment_expr->data.expr_assignment.expression = jo_parse_expression(parser);
	return assigment_expr;
}

// jo_ast_node_t* jo_parse_as_cast(jo_parser_t* parser)
// {
// 	jo_ast_node_t* as_cast_node = jo_make_ast_node(jo_ast_expr_as_cast);
// 	as_cast_node->data.expr_as_cast.expr = jo_parse_expression(parser);
// 	jo_parser_consume()
// }

jo_ast_node_t* jo_parse_primary_expression(jo_parser_t* parser)
{
	if(jo_parser_current(parser)->kind == jo_token_kind_literal
	|| jo_parser_current(parser)->kind == jo_token_kind_type_primitive)
	{
		 return jo_parse_literal_expression(parser);
	}

	switch (jo_parser_current(parser)->type)
	{
	case jo_token_keyword_fn:
		return jo_parse_literal_fn(parser);
		break;
	// case jo_token_keyword_struct:
		// return jo_parse_literal_struct(parser);
		// break;
	case jo_token_identifier:
		if(jo_parser_peek_next(parser)->type == jo_token_colon
		 || jo_parser_peek_next(parser)->type == jo_token_walrus
		 || jo_parser_peek_next(parser)->type == jo_token_bridge)
		{
			return jo_parse_declaration(parser);
		}
		else
		{
			return jo_parse_identifier(parser);
		}
		break;
	case jo_token_open_parenthesis:
		jo_parser_consume(parser, jo_token_open_parenthesis);
		jo_ast_node_t* inner_expr = jo_parse_expression(parser);
		jo_parser_consume(parser, jo_token_close_parenthesis);
		return inner_expr;
		break;

	default:
		jo_parser_unexpected(parser, "expected expression");
		break;
	}
}


jo_ast_node_t* jo_parse_expr_declaration(jo_parser_t* parser)
{
	jo_ast_node_t* statement_declaration_node = jo_make_ast_node(jo_ast_type_expr_decl);

	statement_declaration_node->data.expr_decl.declaration = jo_parse_declaration(parser);

	return statement_declaration_node;
}

// jo_ast_node_t*  jo_parse_expr_for(jo_parser_t* parser)
// {
// 	jo_parser_consume(parser, jo_token_keyword_for);

// 	jo_ast_node_t* statement_for_node = jo_make_ast_node(jo_ast_type_expr_for);

// 	statement_for_node->data.expr_for.iterator = jo_parse_expression(parser);

// 	jo_parser_consume(parser, jo_token_keyword_in);

// 	statement_for_node->data.expr_for.iterable = jo_parse_expression(parser);

// 	statement_for_node->data.expr_for.expression = jo_parse_expression(parser);

// 	return statement_for_node;
// }

jo_ast_node_t* jo_parse_type_fn(jo_parser_t* parser)
{
	jo_parser_consume(parser, jo_token_keyword_fn);

	jo_ast_node_t* type_function_node = jo_make_ast_node(jo_ast_type_type_fn);
	jo_ast_type_fn* type_fn = &type_function_node->data.type_fn;

	if(jo_parser_current(parser)->type == jo_token_open_parenthesis)
	{
		jo_parser_consume(parser, jo_token_open_parenthesis);

		type_fn->parameters = jo_parse_type_list(parser);

		jo_parser_consume(parser, jo_token_close_parenthesis);
	}

	if(jo_parser_current(parser)->type == jo_token_arrow)
	{
		jo_parser_consume(parser, jo_token_arrow);
		
		type_fn->return_type = jo_parse_type(parser);
	}
	else
	{
		type_fn->return_type = jo_make_ast_node(jo_ast_type_type_primitive);
		type_fn->return_type->data.type_primitive = jo_token_keyword_void;
	}

	return type_function_node;
}

jo_ast_node_t* jo_parse_identifier(jo_parser_t* parser)
{
	jo_ast_node_t* identifier_node = jo_make_ast_node(jo_ast_type_identifier);

	jo_token_t* t = jo_parser_consume(parser, jo_token_identifier);

	identifier_node->data.identifier = jo_string_from_n(t->content, t->content_len);

	return identifier_node;
}

jo_ast_node_t* jo_parse_stmt_return(jo_parser_t* parser)
{
	jo_parser_consume(parser, jo_token_keyword_return);
	jo_ast_node_t* stmt_return = jo_make_ast_node(jo_ast_type_stmt_return);
	if(jo_parser_current(parser)->type != jo_token_close_curly_bracket && jo_parser_current(parser)->type != jo_token_semicolon)
	{
		stmt_return->data.stmt_return.expression = jo_parse_expression(parser);
	}
	return stmt_return;
}

jo_ast_node_t* jo_parse_stmt_if(jo_parser_t* parser)
{
	jo_parser_consume(parser, jo_token_keyword_if);

	jo_ast_node_t* if_else_node = jo_make_ast_node(jo_ast_type_stmt_ifelse);

	if_else_node->data.stmt_ifelse.condition = jo_parse_expression(parser);
	if_else_node->data.stmt_ifelse.true_block = jo_parse_block(parser);

	if(jo_parser_current(parser)->type == jo_token_keyword_else)
	{
		jo_parser_consume(parser, jo_token_keyword_else);

		if(jo_parser_current(parser)->type == jo_token_keyword_if)
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

jo_ast_node_t*  jo_parse_stmt_expr(jo_parser_t* parser)
{
	jo_ast_node_t* expr_stmt = jo_make_ast_node(jo_ast_type_stmt_expr);
	expr_stmt->data.stmt_expr.expr = jo_parse_expression(parser);
	return expr_stmt;
}


jo_ast_node_t* jo_parse_stmt(jo_parser_t* parser)
{
	switch (jo_parser_current(parser)->type)
	{
	case jo_token_keyword_if:
		return jo_parse_stmt_if(parser);
		break;

	case jo_token_keyword_return:
		return jo_parse_stmt_return(parser);
		break;

	case jo_token_open_curly_bracket:
		return jo_parse_block(parser);
		break;

	default:
		return jo_parse_stmt_expr(parser);
		break;
	}
}

jo_ast_node_t* jo_parse_block(jo_parser_t* parser)
{
	jo_parser_consume(parser, jo_token_open_curly_bracket);

	jo_ast_node_t* block_node = jo_make_ast_node(jo_ast_type_block);

	while(jo_parser_current(parser)->type != jo_token_close_curly_bracket)
	{
		jo_dyn_array_append(&block_node->data.block.statements, jo_parse_stmt(parser));

		if(jo_parser_current(parser)->type == jo_token_semicolon)
		{
			jo_parser_consume(parser, jo_token_semicolon);
		}
	}

	jo_parser_consume(parser, jo_token_close_curly_bracket);

	return block_node;
}

jo_ast_node_t* jo_parse_declaration(jo_parser_t* parser)
{
	jo_ast_node_t* declaration_node = jo_make_ast_node(jo_ast_type_decl);

	declaration_node->data.decl.identifier = jo_parse_identifier(parser);

	switch (jo_parser_current(parser)->type)
	{
	case jo_token_colon:
		jo_parser_consume(parser, jo_token_colon);
		declaration_node->data.decl.specified_type = jo_parse_type(parser);
		if(jo_parser_current(parser)->type == jo_token_equals)
		{
			jo_parser_consume(parser, jo_token_equals);
			declaration_node->data.decl.initialize_expression = jo_parse_expression(parser);
		}
		else if(jo_parser_current(parser)->type == jo_token_colon)
		{
			jo_parser_consume(parser, jo_token_colon);
			declaration_node->data.decl.initialize_expression = jo_parse_expression(parser);
			declaration_node->data.decl.is_static = true;
		}
		break;
	case jo_token_walrus:
		jo_parser_consume(parser, jo_token_walrus);
		declaration_node->data.decl.initialize_expression = jo_parse_expression(parser);
		break;

	case jo_token_bridge:
		jo_parser_consume(parser, jo_token_bridge);
		declaration_node->data.decl.initialize_expression = jo_parse_expression(parser);
		declaration_node->data.decl.is_static = true;
		break;

	default:
		break;
	}

	return declaration_node;
}


jo_ast_node_ptr_dyn_array_t jo_parse_declarations(jo_parser_t* parser)
{
	jo_ast_node_ptr_dyn_array_t declaration_nodes = {0};

	while(jo_parser_current(parser)->type != jo_token_eof)
	{
		jo_dyn_array_append(&declaration_nodes, jo_parse_declaration(parser));
	}

	return declaration_nodes;
}

jo_ast_node_t* jo_parse_directive(jo_parser_t* parser)
{
	jo_parser_consume(parser, jo_token_hash);
	jo_ast_node_t* directive_node = {0};
	switch (jo_parser_current(parser)->type)
	{
	case jo_token_keyword_load:
		jo_parser_consume(parser, jo_token_keyword_load);
		directive_node = jo_make_ast_node(jo_ast_type_directive_load);
		directive_node->data.directive_load.path = jo_string_from(jo_parser_current(parser)->content);
		jo_parser_consume(parser, jo_token_literal_string);
		break;
	case jo_token_keyword_intrinsic:
		jo_parser_consume(parser, jo_token_keyword_intrinsic);
		directive_node = jo_make_ast_node(jo_ast_type_directive_intrinsic);
		directive_node->data.directive_load.path = jo_string_from(jo_parser_current(parser)->content);
		jo_parser_consume(parser, jo_token_literal_string);
		break;
	default:
		break;
	}

	return directive_node;
}

jo_ast_node_t* jo_parse_module_content(jo_parser_t* parser)
{
	switch (jo_parser_current(parser)->type)
	{
	case jo_token_hash:
		assert(0 && "directive parse");
		// return jo_parse_directive(parser);
		break;

	default:
		return jo_parse_declaration(parser);
		break;
	}
}

jo_ast_node_t* jo_parse_module(jo_parser_t* parser)
{
	jo_ast_node_t* module = jo_make_ast_node(jo_ast_type_module);

	while(jo_parser_current(parser)->type != jo_token_eof)
	{
		jo_dyn_array_append(&module->data.module.content, jo_parse_module_content(parser));
	}

	return module;
}


jo_ast_node_t* jo_parse(jo_parser_t* parser)
{
	return jo_parse_module(parser);
}
