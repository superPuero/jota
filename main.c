#include <stdio.h>
#include <stdlib.h>

#include "compiler/compile_options.h"
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "compiler/symbol.h"
#include "compiler/sema.h"
#include "compiler/bytecode.h"
#include "compiler/vm.h"

void dump_tokens(jo_lexer_t* lexer)
{
	jo_token_da_t* tokens = &lexer->tokens;

	jo_dyn_array_iter(tokens, it,
		{
			switch (tokens->data[it].type)
			{
			case jo_token_identifier:
			case jo_token_literal_integer:
			case jo_token_literal_fp:
				printf("line: %u colum: %u type: %u %s (%.*s)\n", tokens->data[it].line, tokens->data[it].column, tokens->data[it].type, jo_token_type_to_string(tokens->data[it].type), tokens->data[it].content_len, tokens->data[it].content);
				break;

			default:
				printf("line: %u colum: %u type: %u %s\n", tokens->data[it].line,  tokens->data[it].column, tokens->data[it].type, jo_token_type_to_string(tokens->data[it].type));
				break;
			}
		}
	);
}

void dump_bytecode(jo_bytecode_context* bcc)	
{
    printf("-----------bytecode---------\n");
    printf("\n");

	jo_dyn_array_iter(&bcc->bc, j,
		{
			jo_bytecode_op* op = &bcc->bc.data[j];

			jo_dyn_array_iter(&bcc->fns, i,
				{
					if(j == bcc->fns.data[i].entry_ip)
					{
						printf("%s (registers: %u)\n", bcc->fns.data[i].label.data, bcc->fns.data[i].reg_counter);	
					}
				}
			);

			printf("%4zu: ", j);
			jo_bytecode_dump_op(bcc, op);
		}
	);
}


int main(int argc, char** argv)
{	
	jo_compile_options compile_opt = jo_compie_options_parse_from_args(argc, argv);	
	
	if(!compile_opt.success)
	{
		return 1;
	}
		
	jo_arena_t arena = jo_arena_make(jo_Mb(10), "compiler");
	
	//@Important: code repetition is intentional
	if(compile_opt.tokens)
	{
		jo_lexer_t lexer = {.arena = &arena};
		jo_lex_file(&lexer, argv[1]);
		dump_tokens(&lexer);
	}

	if(compile_opt.ast)
	{
		jo_lexer_t lexer = {.arena = &arena};		
		jo_lex_file(&lexer, argv[1]);	
		jo_parser_t parser = { .arena = &arena, .lexer = &lexer };	
		jo_ast_node_t* ast_module = jo_parse(&parser);
		jo_dump_ast_node(ast_module, 0);
	}

	if(compile_opt.sema)
	{
		jo_lexer_t lexer = {.arena = &arena};
		jo_lex_file(&lexer, argv[1]);	
		jo_parser_t parser = { .arena = &arena, .lexer = &lexer };
		jo_ast_node_t* ast_module = jo_parse(&parser);
		jo_sema_t sema = {.arena = &arena};
		jo_sema_analyze(&sema, ast_module);
	}

	if(compile_opt.bytecode)
	{
		jo_lexer_t lexer = {.arena = &arena};
		jo_lex_file(&lexer, argv[1]);	
		jo_parser_t parser = { .arena = &arena, .lexer = &lexer };
		jo_ast_node_t* ast_module = jo_parse(&parser);
		jo_sema_t sema = {.arena = &arena};
		jo_sema_analyze(&sema, ast_module);
		jo_bytecode_context bcc = jo_make_bytecode(&ast_module->data.module);
		dump_bytecode(&bcc);
	}

	if(compile_opt.interp)
	{
		jo_lexer_t lexer = {.arena = &arena};
		jo_lex_file(&lexer, argv[1]);	
		jo_parser_t parser = { .arena = &arena, .lexer = &lexer };
		jo_ast_node_t* ast_module = jo_parse(&parser);
		jo_sema_t sema = {.arena = &arena};
		jo_sema_analyze(&sema, ast_module);
		jo_bytecode_context bcc = jo_make_bytecode(&ast_module->data.module);
		jo_run_bytecode(&bcc);
	}

	jo_arena_free(&arena);
}
