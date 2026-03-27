#include <stdio.h>
#include <stdlib.h>

#include "compiler/compile_options.h"
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "compiler/symbol.h"
#include "compiler/sema.h"
#include "compiler/bytecode.h"
#include "compiler/vm.h"

void dump_tokens(jo_lexer* lexer)
{
	jo_token_ada* tokens = &lexer->tokens;

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
	
	jo_arena arena = jo_arena_make(jo_Mb(1024), "compiler");
	
	jo_lexer lexer = {.arena = &arena};
	jo_f64 lexer_time = 0.0;
	jo_ast_node* ast_module = NULL;
	jo_parser parser = { .arena = &arena, .lexer = &lexer };	
	jo_f64 parser_time = 0.0;
	jo_sema sema = {.arena = &arena};
	jo_f64 sema_time = 0.0;
	jo_bytecode_context bcc = {.arena = &arena};
	jo_f64 bytecode_time = 0.0;
	jo_f64 vm_time = 0.0;

	jo_i64* output = NULL;
	jo_f64 total_time = 0.0;

	if(compile_opt.tokens)
	{
		jo_profile("tokenization", lexer_time)
		{
			jo_lex_file(&lexer, argv[1]);	
		}

		if(compile_opt.tokens_dump) { dump_tokens(&lexer); }		
	}
	
	if(compile_opt.ast)
	{
		jo_profile("ast_generation", parser_time)
		{
			ast_module = jo_parse(&parser);
		}
		if(compile_opt.ast_dump) { jo_dump_ast_node(ast_module, 0); }
	}

	if(compile_opt.sema)
	{
		jo_profile("sema", sema_time)
		{
			jo_sema_analyze(&sema, ast_module);
		}
	}

	if(compile_opt.bytecode)
	{
		jo_profile("bytecode_generation", bytecode_time)
		{
			jo_make_bytecode(&bcc, &ast_module->data.module);
		}	
		if(compile_opt.bytecode_dump) { dump_bytecode(&bcc); }
	}


	if(compile_opt.interp)
	{
		jo_profile("program_runtime", vm_time)
		{
			jo_vm* vm = jo_arena_palloc(&arena, jo_vm);
			output = jo_run_bytecode(vm, &bcc);
		}
	}

	total_time = lexer_time + parser_time + sema_time + bytecode_time + vm_time;
	printf("%s:%*.3fs\n", "total", 30 - (jo_i32)strlen("total"), total_time); 

	if(output) { printf("program output: %llu\n", *output); }

	printf("total memory usage: %.2lfMb", arena.current / 1024.0 / 1024.0);

	jo_arena_free(&arena);
}
