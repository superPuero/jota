#ifndef jota_workspace
#define jota_workspace

#include "ast_node.h"
#include "lexer.h"
#include "parser.h"
#include "bytecode.h"

typedef struct
{	
	token_ada tokens;
	ast_node* file_node;
	astr name;
}module;

ada_declare(module, modules_ada);
ada_declare(astr, load_queue);

struct workspace_s
{
	arena arena;
	astr name;

	load_queue* load_queue;
	astr current_directory;
	modules_ada loaded_modules;	
	scope global_scope;
};

workspace workspace_make(str_view name, uz memory);
void workspace_release(workspace* ws);

void workspace_begin(workspace* workspace, str_view entry_file);
void workspace_load(workspace* workspace, str_view file_path);
void workspace_load_queue(workspace* workspace);
module* workspace_lookup_module(workspace* workspace, str_view name); 

#endif