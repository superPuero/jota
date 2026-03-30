#include <stdio.h>
#include <stdlib.h>

#include "compiler/compile_options.h"
#include "compiler/workspace.h"
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "compiler/symbol.h"
#include "compiler/sema.h"
#include "compiler/bytecode.h"
#include "compiler/vm.h"

#define jo_workspace_memory jo_Mb(10)

// for 1mil lines of code benchmark
// #define jo_workspace_memory jo_Mb(1024)

int main(int argc, char** argv)
{		
	jo_compile_options compile_opt = jo_compile_options_parse_from_args(argc, argv);	
	if(!compile_opt.success) { return 1; }
	
	jo_workspace workspace = jo_workspace_make(jo_str_view_from("main"), jo_workspace_memory);	

	// @todo: bytecode should be per_workspace aka per compilation unit
	jo_bytecode_context bcc = { .ws = &workspace };

	// @explain: jo_vm is huge that why its not stack based
	jo_vm* vm = jo_arena_palloc(&workspace.arena, jo_vm);

	jo_i64* program_output = NULL;

	jo_profiler profiler = {0};
	
	jo_profile(&workspace.arena, &profiler, total_time)
	{
		jo_profile(&workspace.arena, &profiler, lex_and_parse_time) { jo_workspace_begin(&workspace, compile_opt.filepath); }

		if(compile_opt.t)
		{
			jo_ada_foreach(&workspace.loaded_modules) { jo_dump_tokens(&workspace.loaded_modules.it->tokens); }
		} 

		if(compile_opt.ast) 
		{
			jo_ada_foreach(&workspace.loaded_modules) { jo_dump_ast_node(workspace.loaded_modules.it->file_node, 0); }
		}

		jo_sema sema = { .ws = &workspace };
		jo_profile(&workspace.arena, &profiler, sema_time) { jo_sema_analyze(&sema); }

		jo_profile(&workspace.arena, &profiler, bytecode_time) { jo_make_bytecode(&bcc); }	
		if(compile_opt.bc) { jo_dump_bytecode(&bcc); }


		if(compile_opt.i)
		{			
			jo_profile(&workspace.arena, &profiler, vm_time) { program_output = jo_run_bytecode(vm, &bcc); }
		}
	}

	// @explain: during runtime program might output some text, so we go to new line to ensure redability
	printf("\n");

	if(program_output) { printf("program output: %lli\n", *program_output); }

	if(compile_opt.time)
	{
		jo_f64 total_time = 0.0;
		jo_ada_foreach(&profiler) { total_time += profiler.it->time; };
		jo_ada_foreach(&profiler)
		{
			printf("%s:%*.3fs\n", profiler.it->name, 30 - (jo_i32)strlen(profiler.it->name), profiler.it->time); 
		}

	}

	jo_f64 memory_usage_mb = workspace.arena.current / 1024.0 / 1024.0;
	jo_f64 vm_memory_usage_mb = sizeof(jo_vm) / 1024.0 / 1024.0;
	printf("total memory usage: %.2lfMb (%.2lfMb including vm)", memory_usage_mb - vm_memory_usage_mb, memory_usage_mb);

	jo_workspace_free(&workspace);	
}
