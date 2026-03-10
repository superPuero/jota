#include "parser.h"
#include "token.h"

jo_token_t* jo_parser_peek(jo_parser_t* parser, jo_u32 offset)
{
	if(parser->current_token + offset < parser->tokens->occupied)
	{
		return parser->tokens->data + parser->current_token + offset;
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
	return parser->tokens->data + parser->current_token;
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

	jo_parser_consume(parser, jo_token_open_paren);

	while(true)	
	{		
		if(jo_parser_current(parser)->type == jo_token_minus) // minus from return separator arrow (->)
		{			
			break;
		}

		if(jo_parser_current(parser)->type ==  jo_token_close_paren) // implicid -> void 
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
	jo_ast_node_t* type_primitive_node = jo_make_ast_node(jo_ast_type_primitive);
	type_primitive_node->data.type_primitive = jo_parser_current(parser)->type;
	jo_parser_advance(parser);

	return type_primitive_node;
}

jo_ast_node_ptr_dyn_array_t jo_parse_type_extent(jo_parser_t* parser)
{
	return (jo_ast_node_ptr_dyn_array_t){0};
}

jo_ast_node_t* jo_parse_type_tuple(jo_parser_t* parser)
{
	jo_parser_consume(parser, jo_token_open_paren);

	jo_ast_node_t* type_tuple_node = jo_make_ast_node(jo_ast_type_tuple);
	type_tuple_node->data.type_tuple.entries = jo_parse_declaration_list(parser);
	jo_parser_consume(parser, jo_token_close_paren);

	return type_tuple_node;
}

jo_ast_node_t* jo_parse_type_struct(jo_parser_t* parser)
{
	jo_parser_consume(parser, jo_token_keyword_struct);

	jo_ast_node_t* type_struct_node = jo_make_ast_node(jo_ast_type_struct);

	jo_parser_consume(parser, jo_token_open_paren);

	type_struct_node->data.type_struct.members = jo_parse_declaration_list(parser);	
					
	jo_parser_consume(parser, jo_token_close_paren);

	return type_struct_node;
}

jo_ast_node_t* jo_parse_type(jo_parser_t* parser)
{
	jo_ast_node_t* type_node = jo_make_ast_node(jo_ast_type);

	switch (jo_parser_current(parser)->kind)
	{
	case jo_token_kind_type_primitive:
		type_node->data.type.inner = jo_parse_type_primitive(parser);
		break;

	default:
		break;
	}

	// ------- type extent(pointer *, reference &, array [n]) --------- 
	switch (jo_parser_current(parser)->type)
	{
	case jo_token_open_square_bracket:
		jo_parser_consume(parser, jo_token_open_square_bracket);
		type_node->data.type.array = true;
		type_node->data.type.array_size_expression = jo_parse_expression(parser);
		jo_parser_consume(parser, jo_token_close_square_bracket);
		type_node->data.type.inner = jo_parse_type(parser);
		break;
					
	case jo_token_star:
		type_node->data.type.pointer = true;
		jo_parser_advance(parser);
		type_node->data.type.inner = jo_parse_type(parser);
		break;
	case jo_token_ampersand:
		type_node->data.type.reference = true;
		jo_parser_advance(parser);
		type_node->data.type.inner = jo_parse_type(parser);
		break;
	}
	//------------------------------------------------------------------

	
	switch (jo_parser_current(parser)->type)
	{
	case jo_token_open_paren:
		type_node->data.type.inner = jo_parse_type_tuple(parser);		
		break;	
			
	case jo_token_identifier:
		type_node->data.type.inner = jo_parse_identifier(parser);
		break;

	case jo_token_keyword_fn:
		type_node->data.type.inner = jo_parse_type_function(parser);		
		break;
	
	case jo_token_keyword_struct:
		type_node->data.type.inner = jo_parse_type_struct(parser);		
		break;	

	}

	if(!type_node->data.type.inner)
	{
		jo_parser_unexpected(parser, "expected type");
	}

	return type_node;
}

jo_ast_node_t* jo_parse_literal_integer(jo_parser_t* parser)
{
	jo_ast_node_t* literal_integer_node = jo_make_ast_node(jo_ast_literal_integer);
	literal_integer_node->data.literal_integer = atoi(jo_parser_current(parser)->content);
	jo_parser_advance(parser);
	return literal_integer_node;
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
	case jo_token_open_paren:
		jo_parser_consume(parser, jo_token_open_paren);
		jo_ast_node_t* apply_paren_operation_node = jo_make_ast_node(jo_ast_apply_operation);
		apply_paren_operation_node->data.operation_apply.apply_type = jo_token_open_paren;
		apply_paren_operation_node->data.operation_apply.target = expression;									
		if(jo_parser_current(parser)->type != jo_token_close_paren)
		{
			apply_paren_operation_node->data.operation_apply.arguments = jo_parse_expression_list(parser);
		}
		jo_parser_consume(parser, jo_token_close_paren);
		return apply_paren_operation_node;
		break;

	case jo_token_open_square_bracket:
		jo_parser_consume(parser, jo_token_open_square_bracket);
		jo_ast_node_t* apply_squaer_brackets_operation_node = jo_make_ast_node(jo_ast_apply_operation);
		apply_squaer_brackets_operation_node->data.operation_apply.apply_type = jo_token_open_square_bracket;
		apply_squaer_brackets_operation_node->data.operation_apply.target = expression;		
		if(jo_parser_current(parser)->type != jo_token_close_square_bracket)
		{
			apply_squaer_brackets_operation_node->data.operation_apply.arguments = jo_parse_expression_list(parser);	
		}
		jo_parser_consume(parser, jo_token_close_square_bracket);
		return apply_squaer_brackets_operation_node;
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

		do
		{
			postfix_expression = jo_try_parse_expression_postfix_operator(parser, left_expression_node);
	
			if(postfix_expression)
			{
				left_expression_node = postfix_expression;
			}
		}while(postfix_expression);

		jo_token_type_t operator_type = jo_parser_current(parser)->type;
		jo_u32 precedence = jo_token_binary_operator_precedence(operator_type);
		
		if(precedence == 0) // not an operator, statemt end 
		{
			break;	
		}
		
		if(precedence < min_precedence)
		{
			break;
		}

		jo_parser_advance(parser);		
		
		jo_ast_node_t* right_expression_node = jo_parse_expression_precedented(parser, precedence + 1);

		jo_ast_node_t* new_left_expression_node = jo_make_ast_node(jo_ast_binary_opperation);

		new_left_expression_node->data.operation_binary.operator_type = operator_type;
		new_left_expression_node->data.operation_binary.left_expression = left_expression_node;
		new_left_expression_node->data.operation_binary.right_expression = right_expression_node;
	
		left_expression_node = new_left_expression_node;	
	}
	

	return left_expression_node;
}


jo_ast_node_t* jo_parse_expression(jo_parser_t* parser)
{
	return jo_parse_expression_precedented(parser, 0);
}

jo_ast_node_t* jo_parse_parse_if_expression(jo_parser_t* parser)
{
	jo_parser_consume(parser, jo_token_keyword_if);

	jo_ast_node_t* expression_if_node = jo_make_ast_node(jo_ast_expression_if);

	expression_if_node->data.expression_if.condition = jo_parse_expression(parser);
	expression_if_node->data.expression_if.true_branch = jo_parse_expression(parser);	

	if(jo_parser_current(parser)->type == jo_token_keyword_else)
	{
		jo_parser_consume(parser, jo_token_keyword_else);
		expression_if_node->data.expression_if.false_branch = jo_parse_expression(parser);
	}

	return expression_if_node;
}

jo_ast_node_t* jo_parse_primary_expression(jo_parser_t* parser)
{
	switch (jo_parser_current(parser)->type)
	{
	case jo_token_open_paren:
		jo_parser_consume(parser, jo_token_open_paren);
        jo_ast_node_t* inner_expr = jo_parse_expression(parser);
        jo_parser_consume(parser, jo_token_close_paren);        
        return inner_expr;
		break;

	case jo_token_keyword_if:
		return jo_parse_parse_if_expression(parser);
		break;

	case jo_token_literal_integer:		
		return jo_parse_literal_integer(parser);
		break;
	case jo_token_identifier:
		return jo_parse_identifier(parser);
		break;
	case jo_token_open_curly_bracket:
		return jo_parse_block(parser);
		break;
	
	default:
		jo_parser_unexpected(parser, "expected expression");
		break;
	}
}


jo_ast_node_t* jo_parse_statement_return(jo_parser_t* parser)
{
	jo_parser_consume(parser, jo_token_keyword_return);
	
	jo_ast_node_t* statement_return_node = jo_make_ast_node(jo_ast_statement_return);

	statement_return_node->data.statement_return.expression = jo_parse_expression(parser);

	return statement_return_node;
}

jo_ast_node_t* jo_parse_statement_declaration(jo_parser_t* parser)
{
	jo_ast_node_t* statement_declaration_node = jo_make_ast_node(jo_ast_statement_declaration);

	statement_declaration_node->data.statement_declaration.declaration = jo_parse_declaration(parser);

	return statement_declaration_node;
}

jo_ast_node_t* jo_parse_statement_expression(jo_parser_t* parser)
{
	jo_ast_node_t* statement_expression_node = jo_make_ast_node(jo_ast_statement_expression);

	statement_expression_node->data.statement_expression.expression = jo_parse_expression(parser);

	return statement_expression_node;
}

jo_ast_node_t* jo_parse_statement(jo_parser_t* parser)
{
	switch (jo_parser_current(parser)->type)
	{
	case jo_token_keyword_return:
		return jo_parse_statement_return(parser);
		break;
	case jo_token_identifier:
		if(jo_parser_peek_next(parser)->type == jo_token_colon)
		{
			return jo_parse_statement_declaration(parser);
		}
		else
		{
			return jo_parse_statement_expression(parser);
		}
		break;		
		
	default:
		return jo_parse_statement_expression(parser);
		break;
	}
}

jo_ast_node_t* jo_parse_block(jo_parser_t* parser)
{	
	jo_parser_consume(parser, jo_token_open_curly_bracket);
	jo_ast_node_t* block_node = jo_make_ast_node(jo_ast_block);
    
	while(true)
    {
		if(jo_parser_current(parser)->type == jo_token_close_curly_bracket)
		{
			jo_parser_consume(parser, jo_token_close_curly_bracket);
			break;
		}

		jo_ast_node_t* statement_node = jo_parse_statement(parser);
				
		if(statement_node->type == jo_ast_statement_expression && jo_parser_current(parser)->type != jo_token_semicolon)
		{
			block_node->data.block.yield_expression = statement_node->data.statement_expression.expression;
			free(statement_node);
		}
		else
		{
			if(block_node->data.block.yield_expression)
			{
				jo_err("block may not have eny statement after yielding expression%s", "\n");
			} 
			jo_dyn_array_append(&block_node->data.block.statements, statement_node);
			jo_parser_consume(parser, jo_token_semicolon);
		}		
    }
	
	return block_node;
}


jo_ast_node_t* jo_parse_type_function(jo_parser_t* parser)
{
	jo_parser_consume(parser, jo_token_keyword_fn);

	jo_ast_node_t* type_function_node = jo_make_ast_node(jo_ast_type_function);
	
	if(jo_parser_current(parser)->type == jo_token_open_paren)
	{
		jo_parser_consume(parser, jo_token_open_paren);

		if(jo_parser_current(parser)->type != jo_token_close_paren)
		{
			type_function_node->data.type_function.parameters = jo_parse_declaration_list(parser);
				
			// ->
			if(jo_parser_current(parser)->type == jo_token_minus)
			{
				jo_parser_consume(parser, jo_token_minus);
				jo_parser_consume(parser, jo_token_close_angle_bracket);
				type_function_node->data.type_function.return_type = jo_parse_type(parser);
			}
		}
		
		jo_parser_consume(parser, jo_token_close_paren);
	}

	
	return type_function_node;
}

jo_ast_node_t* jo_parse_construct_fn(jo_parser_t* parser)
{	
	jo_ast_node_t* construct_fn_node = jo_make_ast_node(jo_ast_construct_fn);
	
	construct_fn_node->data.construct_fn.type = jo_parse_type_function(parser);
	construct_fn_node->data.construct_fn.body_expression = jo_parse_expression(parser);
	
	return construct_fn_node;
}

jo_ast_node_t* jo_parse_identifier(jo_parser_t* parser)
{
	jo_ast_node_t* identifier_node = jo_make_ast_node(jo_ast_identifier);

	memcpy(identifier_node->data.identifier, 
		jo_parser_consume(parser, jo_token_identifier)->content,
		jo_token_content_max_length);
		
	return identifier_node;
}

bool jo_parser_at_declaration(jo_parser_t* parser)
{
	return jo_parser_current(parser)->type == jo_token_identifier && jo_parser_peek_next(parser)->type == jo_token_colon;
}

bool jo_parser_at_bridge(jo_parser_t* parser)
{
	return jo_parser_current(parser)->type == jo_token_colon && jo_parser_peek_next(parser)->type == jo_token_colon;
}
	
jo_ast_node_t* jo_parse_construct(jo_parser_t* parser)
{
	jo_ast_node_t* construct_variable = jo_make_ast_node(jo_ast_construct_variable);
	construct_variable->data.construct_variable.type = jo_parse_type(parser);
	
	if(jo_parser_current(parser)->type == jo_token_equals)
	{
		jo_parser_consume(parser, jo_token_equals);				
		construct_variable->data.construct_variable.initializing_expression = jo_parse_expression(parser);
	}
	// alternative assigment syntax (x: u32 :: 42)
	else if(jo_parser_at_bridge(parser))
	{
		jo_parser_consume(parser, jo_token_colon);
		jo_parser_consume(parser, jo_token_colon);				

		construct_variable->data.construct_variable.initializing_expression = jo_parse_expression(parser);
	}	

	return construct_variable;
}

jo_ast_node_t* jo_parse_construct_non_variable(jo_parser_t* parser)
{
	switch (jo_parser_current(parser)->type)
	{
	case jo_token_keyword_fn:
		return jo_parse_construct_fn(parser);
		break;	
	
	default:
		jo_parser_unexpected(parser, "expected non variable construct declaration");
		break;
	}
}

jo_ast_node_t* jo_parse_declaration(jo_parser_t* parser)
{
	jo_ast_node_t* declaration_node = jo_make_ast_node(jo_ast_declaration);

	if(jo_parser_at_declaration(parser))
	{
		declaration_node->data.declaration.identifier = jo_parse_identifier(parser);
		jo_parser_consume(parser, jo_token_colon);
	}

	declaration_node->data.declaration.construct = jo_parse_construct(parser);

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

jo_ast_node_t* jo_parse(jo_parser_t* parser)
{
	jo_ast_node_t* entry_node = jo_make_ast_node(jo_ast_entry);

	entry_node->data.entry.declarations = jo_parse_declarations(parser);

	return entry_node;
}