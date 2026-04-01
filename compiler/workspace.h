#ifndef jota_workspace
#define jota_workspace

#include "ast_node.h"
#include "lexer.h"
#include "parser.h"
#include "bytecode.h"

typedef struct
{	
	token_da tokens;
	ast_node* file_node;
	str name;
}module;

da_declare(module, modules_da);
da_declare(str, load_queue);

struct workspace_s
{
	arena arena;
	str name;

	load_queue* load_queue;
	str current_directory;
	modules_da loaded_modules;	
	scope global_scope;
};

workspace workspace_make(strv name, uz memory);
void workspace_release(workspace* ws);

void workspace_begin(workspace* workspace, strv entry_file);
void workspace_load(workspace* workspace, strv file_path);
void workspace_load_queue(workspace* workspace);
module* workspace_lookup_module(workspace* workspace, strv name); 

#endif