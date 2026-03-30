#include "workspace.h"

#ifdef jota_platform_windows
    #include <stdlib.h>
    #define jota_max_path _MAX_PATH
#else
    #include <stdlib.h>
    #include <limits.h>
    #define jota_max_path PATH_MAX
#endif

jo_workspace jo_workspace_make(jo_str_view name, jo_uz memory)
{
	jo_workspace out = {0};
	out.arena = jo_arena_make(memory, "");
	memcpy(out.arena.name, name.data, name.len);
	out.name = jo_astr_from_view(&out.arena, name);
	return out;
}

void jo_workspace_free(jo_workspace* ws)
{
	jo_arena_free(&ws->arena);
}

// @TODO: move to separate place
jo_astr jo_get_absolute_path(jo_workspace* ws, jo_str_view relative) 
{
	jo_arena_marker(&ws->arena);
    char temp[jota_max_path];
    
	jo_astr nullt_str = jo_astr_from_view_nt(&ws->arena, relative);
	jo_astr_append(&ws->arena, &nullt_str, "\0");

	char* res = NULL;

	#ifdef jota_platform_windows
		res = _fullpath(temp, nullt_str.data, jota_max_path);
	#else
		res = realpath(nullt_str.data, temp);
	#endif

	jo_arena_pop_to_marker(&ws->arena);
	if (res) 
	{       
        return jo_astr_from(&ws->arena, temp); 
    }

    return jo_astr_from(&ws->arena, "");
}

jo_astr jo_get_file_directory(jo_workspace* ws, const jo_astr* relative) 
{
	jo_arena_marker(&ws->arena);

    char temp_full[jota_max_path];
    
    jo_astr nullt_str = jo_astr_clone(&ws->arena, relative);

    char* res = NULL;
    #ifdef jota_platform_windows
        res = _fullpath(temp_full, nullt_str.data, jota_max_path);
    #else
        res = realpath(nullt_str.data, temp_full);
    #endif

    if (res) 
    {
        #ifdef jota_platform_windows
            char drive[_MAX_DRIVE];
            char dir[_MAX_DIR];
            _splitpath_s(temp_full, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);
            
            char final_dir[jota_max_path];
            _makepath_s(final_dir, jota_max_path, drive, dir, NULL, NULL);			
			jo_arena_pop_to_marker(&ws->arena);

            return jo_astr_from(&ws->arena, final_dir);
        #else
            char* dir_part = dirname(temp_full); 
			jo_arena_pop_to_marker(ws->arena);

            return jo_astr_from(ws->arena, dir_part);
        #endif
    }

	jo_arena_pop_to_marker(&ws->arena);

    return jo_astr_from(&ws->arena, "");
}

void jo_workspace_begin(jo_workspace* workspace, jo_str_view entry_file)
{
	jo_astr abs = jo_get_absolute_path(workspace, entry_file);
	
	workspace->load_queue = jo_arena_palloc(&workspace->arena, jo_load_queue);
	workspace->current_directory = jo_get_file_directory(workspace, &abs);
	
	jo_workspace_load(workspace, &abs);

	while(workspace->load_queue->occupied) { jo_workspace_load_queue(workspace); }
}

void jo_workspace_load(jo_workspace* workspace, const jo_astr* file_path)
{	
	printf("loading %.*s into workspace(\"%.*s\")\n", jo_astr_fmt(file_path),  jo_astr_fmt(&workspace->name));
	workspace->current_directory = jo_get_file_directory(workspace, file_path);

	jo_ada_append(
		&workspace->arena, 
		&workspace->loaded_modules, 
		(jo_module) {
			.name = jo_astr_clone(&workspace->arena, file_path)
		}
	);
	
	jo_module* current_module = jo_ada_last(&workspace->loaded_modules);
	jo_file* file = jo_file_load(&workspace->arena, file_path);

	if(!file)
	{
		printf("%.*s file was not found\n", jo_astr_fmt(file_path));
		assert(0);
	}

	jo_lexer lexer = {
		.arena = &workspace->arena, 
		.file = file,
		.out = &current_module->tokens
	};

	jo_lexer_lex(&lexer);

	jo_parser parser = {
		.arena = &workspace->arena,
		.tokens = &current_module->tokens
	};

	current_module->file_node = jo_parse(workspace, &parser);
}

void jo_workspace_load_queue(jo_workspace* workspace)
{	
	jo_load_queue* old_queue = workspace->load_queue;
	workspace->load_queue = jo_arena_palloc(&workspace->arena, jo_load_queue);

	jo_ada_foreach(old_queue)
	{		
		jo_astr abs_path = jo_get_absolute_path(workspace, jo_str_view_from_astr(old_queue->it));

		if(!jo_workspace_lookup_module(workspace, &abs_path)) { jo_workspace_load(workspace, &abs_path); }
		else { printf("%.*s is already loaded\n", jo_astr_fmt(&abs_path)); }
	}
}

jo_module* jo_workspace_lookup_module(jo_workspace* workspace, const jo_astr* name)
{
	jo_modules_ada* modules = &workspace->loaded_modules;

	jo_ada_foreach(modules)
	{	
		if(strncmp(modules->it->name.data, name->data, name->occupied) == 0) { return modules->it; }
	};

	return NULL;
}