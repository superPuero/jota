#ifndef jota_parser
#define jota_parser

#include <stdlib.h>
#include "compiler_fwd.h"
#include "ast_node.h"
#include "lexer.h"

typedef struct
{
	jo_arena* arena;
	jo_token_ada* tokens;
	jo_u32 current_token;
} jo_parser;

jo_ast_node* jo_parse_stmt_block(jo_parser* parser);
jo_token* jo_parser_peek(jo_parser* parser, jo_u32 offset);
jo_token* jo_parser_peek_next(jo_parser* parser);
void jo_parser_advance(jo_parser* parser);
jo_token* jo_parser_current(jo_parser* parser);
void jo_parser_unexpected(jo_parser* parser, const char* err);
jo_token* jo_parser_consume(jo_parser* parser, jo_tok expected);
jo_ast_node_ptr_ada jo_parse_construct_fn_parameters(jo_parser* parser);
jo_ast_node* jo_parse_type_primitive(jo_parser* parser);
jo_ast_node_ptr_ada jo_parse_type_extent(jo_parser* parser);
jo_ast_node_ptr_ada jo_parse_type_function_parameters(jo_parser* parser);
jo_ast_node_ptr_ada jo_parse_expression_list(jo_parser* parser);
jo_ast_node_ptr_ada jo_parse_declaration_list(jo_parser* parser);
jo_ast_node* jo_parse_type_fn(jo_parser* parser);
jo_ast_node* jo_parse_type(jo_parser* parser);
jo_ast_node* jo_parser_construct_fn_return_type(jo_parser* parser);
jo_ast_node* jo_parse_literal_integer(jo_parser* parser);
jo_ast_node* jo_parse_expression_precedented(jo_parser* parser, jo_u32 min_precedence);
jo_ast_node* jo_parse_expression(jo_parser* parser);
jo_ast_node* jo_parse_primary_expression(jo_parser* parser);
jo_ast_node* jo_parse_expr_return(jo_parser* parser);
jo_ast_node* jo_parse_expr_declaration(jo_parser* parser);
jo_ast_node* jo_parse_expr_for(jo_parser* parser);
jo_ast_node* jo_parse_block(jo_parser* parser);
jo_ast_node* jo_parse_construct(jo_parser* parser);
jo_ast_node* jo_parse_construct_variable(jo_parser* parser);
jo_ast_node* jo_parse_construct_non_variable(jo_parser* parser);
jo_ast_node* jo_parse_construct_fn(jo_parser* parser);
jo_ast_node* jo_parse_identifier(jo_parser* parser);
jo_ast_node* jo_parse_declaration(jo_parser* parser);
jo_ast_node_ptr_ada jo_parse_declarations(jo_parser* parser);
jo_ast_node_ptr_ada jo_parse_namespace_content(jo_parser* parser);
jo_ast_node* jo_parse(jo_workspace* workspace, jo_parser* parser);

#endif