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

	jo_ada_foreach(tokens)
	{
		switch (tokens->it->type)
		{
		case jo_token_identifier:
		case jo_token_literal_integer:
		case jo_token_literal_fp:
			printf("line: %u colum: %u type: %u %s (%.*s)\n", tokens->it->line, tokens->it->column, tokens->it->type, jo_token_type_to_string(tokens->it->type), tokens->it->content_len, tokens->it->content);
			break;

		default:
			printf("line: %u colum: %u type: %u %s\n", tokens->it->line,  tokens->it->column, tokens->it->type, jo_token_type_to_string(tokens->it->type));
			break;
		}
	}
}

void dump_bytecode(jo_bytecode_context* bcc)	
{
    printf("-----------bytecode---------\n");
    printf("\n");

	jo_uz inst = 0;
	jo_ada_foreach(&bcc->bc)
	{
		jo_bytecode_op* op = bcc->bc.it;

		jo_ada_foreach(&bcc->fns)
		{
			if(inst == bcc->fns.it->entry_ip)
			{
				printf("%.*s (registers: %u)\n", bcc->fns.it->label.len, bcc->fns.it->label.data, bcc->fns.it->reg_counter);	
			}
		}

		printf("%4zu: ", inst);
		jo_bytecode_dump_op(bcc, op);				
		inst++;
	}
}

int main(int argc, char** argv)
{		
	jo_compile_options compile_opt = jo_compie_options_parse_from_args(argc, argv);	
	if(!compile_opt.success) { return 1; }
	
	jo_arena_t arena = jo_arena_make(jo_Mb(256), "compiler");
	
	jo_lexer_t lexer = {.arena = &arena};
	jo_ast_node_t* ast_module = NULL;
	jo_parser_t parser = { .arena = &arena, .lexer = &lexer };	
	jo_sema_t sema = {.arena = &arena};
	jo_bytecode_context bcc = {.arena = &arena};
	
	if(compile_opt.tokens)
	{
		jo_profile("tokenization")
		{
			jo_lex_file(&lexer, argv[1]);	
		}

		if(compile_opt.tokens_dump) { dump_tokens(&lexer); }		
	}
	
	if(compile_opt.ast)
	{
		jo_profile("ast_generation")
		{
			ast_module = jo_parse(&parser);
		}
		if(compile_opt.ast_dump) { jo_dump_ast_node(ast_module, 0); }
	}

	if(compile_opt.sema)
	{
		jo_profile("sema")
		{
			jo_sema_analyze(&sema, ast_module);
		}
	}

	if(compile_opt.bytecode)
	{
		jo_profile("bytecode_generation")
		{
			jo_make_bytecode(&bcc, &ast_module->data.module);
		}
		if(compile_opt.bytecode_dump) { dump_bytecode(&bcc); }
	}

	if(compile_opt.interp)
	{
		jo_profile("program_runtime")
		{
			jo_run_bytecode(&bcc);
		}
	}

	printf("total memory usage %lluKb", arena.current / 1024);

	jo_arena_free(&arena);
}
