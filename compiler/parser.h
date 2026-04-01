#ifndef jota_parser
#define jota_parser

#include <stdlib.h>
#include "compiler_fwd.h"
#include "ast_node.h"
#include "lexer.h"

typedef struct
{
	arena* arena;
	token_ada* tokens;
	u32 current_token;
} parser;

ast_node* parse_stmt_block(parser* parser);
token* parser_peek(parser* parser, u32 offset);
token* parser_peek_next(parser* parser);
void parser_advance(parser* parser);
token* parser_current(parser* parser);
void parser_unexpected(parser* parser, const char* err);
token* parser_consume(parser* parser, tok expected);
ast_node_ptr_ada parse_construct_fn_parameters(parser* parser);
ast_node* parse_type_primitive(parser* parser);
ast_node_ptr_ada parse_type_extent(parser* parser);
ast_node_ptr_ada parse_type_function_parameters(parser* parser);
ast_node_ptr_ada parse_expression_list(parser* parser);
ast_node_ptr_ada parse_declaration_list(parser* parser);
ast_node* parse_type_fn(parser* parser);
ast_node* parse_type(parser* parser);
ast_node* parser_construct_fn_return_type(parser* parser);
ast_node* parse_literal_integer(parser* parser);
ast_node* parse_expression_precedented(parser* parser, u32 min_precedence);
ast_node* parse_expression(parser* parser);
ast_node* parse_primary_expression(parser* parser);
ast_node* parse_expr_return(parser* parser);
ast_node* parse_expr_declaration(parser* parser);
ast_node* parse_expr_for(parser* parser);
ast_node* parse_block(parser* parser);
ast_node* parse_construct(parser* parser);
ast_node* parse_construct_variable(parser* parser);
ast_node* parse_construct_non_variable(parser* parser);
ast_node* parse_construct_fn(parser* parser);
ast_node* parse_identifier(parser* parser);
ast_node* parse_declaration(parser* parser);
ast_node_ptr_ada parse_declarations(parser* parser);
ast_node_ptr_ada parse_namespace_content(parser* parser);
ast_node* parse(workspace* workspace, parser* parser);

#endif