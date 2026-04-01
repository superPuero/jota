#include "compiler/compile_options.h"
#include "compiler/workspace.h"
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "compiler/symbol.h"
#include "compiler/sema.h"
#include "compiler/bytecode.h"
#include "compiler/vm.h"

#define workspace_memory Mb(10)

// for 1mil lines of code benchmark
// #define workspace_memory Mb(1024)

int main(int argc, char** argv)
{			
	compile_options compile_opt = compile_options_parse_from_args(argc, argv);	
	if(!compile_opt.success) { return 1; }
	
	workspace ws = workspace_make(strv_from_cstr("main"), workspace_memory);	

	// @explain: vm is huge that why its not stack allocated
	// also preferrably should be allocated on demand
	vm* virtual_machine = arena_ppush(&ws.arena, vm);

	// @todo: bytecode_context and sema should be per workspace aka per build unit
	bytecode_context bcc = { .ws = &ws };
	sema sema = { .ws = &ws };

	profiler profiler = {0};
	
	if(compile_opt.file_provided)
	{
		profile(&ws.arena, &profiler, total_compiler_time)
		{
			profile(&ws.arena, &profiler, lex_and_parse_time) { workspace_begin(&ws, compile_opt.file); }
			profile(&ws.arena, &profiler, sema_time) { sema_analyze(&sema); }
			profile(&ws.arena, &profiler, bytecode_time) { make_bytecode(&bcc); }	
		}
	}
	
	if(compile_opt.time)
	{
		da_foreach(&profiler)
		{
			printf("%s:%*.3fs\n", profiler.it->name, 30 - (i32)strlen(profiler.it->name), profiler.it->time); 
		}
	}

	if(compile_opt.h)
	{	
		#define XY(opt, help_info) printf("%-10s %s\n", "-"#opt, help_info);
		compile_options_list
		#undef XY
	}

	if(compile_opt.mem)
	{
		f64 memory_usage_mb = ws.arena.current / 1024.0 / 1024.0;
		f64 vm_memory_usage_mb = sizeof(vm) / 1024.0 / 1024.0;

		printf("memory usage: %.2lfMb (%.2lfMb including vm)\n", memory_usage_mb - vm_memory_usage_mb, memory_usage_mb);
	}

	if(compile_opt.i)
	{	
		i64* program_output = NULL;

		program_output = run_bytecode(virtual_machine, &bcc);

		printf("\n");		

		if(program_output) { printf("program output: %lli\n", *program_output); }
	}

	if(compile_opt.t)
	{
		da_foreach(&ws.loaded_modules) { dump_tokens(&ws.loaded_modules.it->tokens); }
	} 

	if(compile_opt.ast) 
	{
		da_foreach(&ws.loaded_modules) { dump_ast_node(ws.loaded_modules.it->file_node, 0); }
	}

	if(compile_opt.bc) { dump_bytecode(&bcc); }

	workspace_release(&ws);	

	return 0;
}
