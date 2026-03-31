#ifndef jota_workspace
#define jota_workspace

#include "ast_node.h"
#include "lexer.h"
#include "parser.h"
#include "bytecode.h"

typedef struct
{	
	jo_token_ada tokens;
	jo_ast_node* file_node;
	jo_astr name;
}jo_module;

jo_ada_declare(jo_module, jo_modules_ada);
jo_ada_declare(jo_astr, jo_load_queue);

struct jo_workspace_s
{
	jo_arena arena;
	jo_astr name;

	jo_load_queue* load_queue;
	jo_astr current_directory;
	jo_modules_ada loaded_modules;	
	jo_scope global_scope;
};

jo_workspace jo_workspace_make(jo_str_view name, jo_uz memory);
void jo_workspace_free(jo_workspace* ws);

void jo_workspace_begin(jo_workspace* workspace, jo_str_view entry_file);
void jo_workspace_load(jo_workspace* workspace, const jo_astr* file_path);
void jo_workspace_load_queue(jo_workspace* workspace);
jo_module* jo_workspace_lookup_module(jo_workspace* workspace, const jo_astr* name); 

#endif