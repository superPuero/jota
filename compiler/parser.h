#ifndef jota_parser
#define jota_parser

#include <stdlib.h>
#include "ast_node.h"
#include "token.h"

typedef struct
{
	jo_token_dyn_array_t* tokens;
	jo_u32 current_token;
} jo_parser_t;

jo_token_t* jo_parser_peek(jo_parser_t* parser, jo_u32 offset);
jo_token_t* jo_parser_peek_next(jo_parser_t* parser);
void jo_parser_advance(jo_parser_t* parser);
jo_token_t* jo_parser_current(jo_parser_t* parser);
void jo_parser_unexpected(jo_parser_t* parser, const char* err);
jo_ast_node_t* jo_make_ast_node(jo_ast_node_type_t type);
jo_token_t* jo_parser_consume(jo_parser_t* parser, jo_token_type_t expected);
jo_ast_node_ptr_dyn_array_t jo_parse_construct_fn_parameters(jo_parser_t* parser);
jo_ast_node_t* jo_parse_type_primitive(jo_parser_t* parser);
jo_ast_node_ptr_dyn_array_t jo_parse_type_extent(jo_parser_t* parser);
jo_ast_node_ptr_dyn_array_t jo_parse_type_function_parameters(jo_parser_t* parser);
jo_ast_node_ptr_dyn_array_t jo_parse_expression_list(jo_parser_t* parser);
jo_ast_node_ptr_dyn_array_t jo_parse_declaration_list(jo_parser_t* parser);
jo_ast_node_t* jo_parse_type_function_return_type(jo_parser_t* parser);
jo_ast_node_t* jo_parse_type_function(jo_parser_t* parser);
jo_ast_node_t* jo_parse_type(jo_parser_t* parser);
jo_ast_node_t* jo_parser_construct_fn_return_type(jo_parser_t* parser);
jo_ast_node_t* jo_parse_literal_integer(jo_parser_t* parser);
jo_ast_node_t* jo_parse_expression_precedented(jo_parser_t* parser, jo_u32 min_precedence);
jo_ast_node_t* jo_parse_expression(jo_parser_t* parser);
jo_ast_node_t* jo_parse_primary_expression(jo_parser_t* parser);
jo_ast_node_t* jo_parse_return_statement(jo_parser_t* parser);
jo_ast_node_t* jo_parse_statement(jo_parser_t* parser);
jo_ast_node_t* jo_parse_block(jo_parser_t* parser);
jo_ast_node_t* jo_parse_construct(jo_parser_t* parser);
jo_ast_node_t* jo_parse_construct_variable(jo_parser_t* parser);
jo_ast_node_t* jo_parse_construct_non_variable(jo_parser_t* parser);
jo_ast_node_t* jo_parse_construct_fn(jo_parser_t* parser);
jo_ast_node_t* jo_parse_identifier(jo_parser_t* parser);
jo_ast_node_t* jo_parse_declaration(jo_parser_t* parser);
jo_ast_node_ptr_dyn_array_t jo_parse_declarations(jo_parser_t* parser);
jo_ast_node_t* jo_parse(jo_parser_t* parser);
#endif