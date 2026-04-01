#include "workspace.h"

#ifdef jota_platform_windows
    #include <stdlib.h>
    #define jota_max_path _MAX_PATH
#else
    #include <stdlib.h>
    #include <limits.h>
    #define jota_max_path PATH_MAX
#endif

workspace workspace_make(strv name, uz memory)
{
	workspace out = {0};
	out.arena = arena_make(memory, name);
	out.name = str_from_view(&out.arena, name);
	return out;
}

void workspace_release(workspace* ws)
{
	arena_release(&ws->arena);
}

// @TODO: move to separate place
str get_absolute_path(workspace* ws, strv relative) 
{
	marker mark = arena_mark(&ws->arena);

    char temp[jota_max_path];
    
	str nullt_str = str_from_view_nt(&ws->arena, relative);
	str_append_cstr(&ws->arena, &nullt_str, "\0");

	char* res = NULL;

	#ifdef jota_platform_windows
		res = _fullpath(temp, nullt_str.data, jota_max_path);
	#else
		res = realpath(nullt_str.data, temp);
	#endif

	arena_pop_to_marker( mark);
	if (res) 
	{       
        return str_from_cstr(&ws->arena, temp); 
    }

    return str_from_cstr(&ws->arena, "");
}

str get_file_directory(workspace* ws, strv relative) 
{
	marker mark = arena_mark(&ws->arena);

    char temp_full[jota_max_path];
    
    str nullt_str = str_from_view_nt(&ws->arena, relative);

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
			arena_pop_to_marker( mark);

            return str_from_cstr(&ws->arena, final_dir);
        #else
            char* dir_part = dirname(temp_full); 
			arena_pop_to_marker(ws->arena, mark);

            return str_from(ws->arena, dir_part);
        #endif
    }

	arena_pop_to_marker( mark);

    return str_from_cstr(&ws->arena, "");
}

void workspace_begin(workspace* workspace, strv entry_file)
{
	str abs = get_absolute_path(workspace, entry_file);
	
	workspace->load_queue = arena_ppush(&workspace->arena, load_queue);
	workspace->current_directory = get_file_directory(workspace, strv_from_str(&abs));
	
	workspace_load(workspace, strv_from_str(&abs));

	while(workspace->load_queue->occupied) { workspace_load_queue(workspace); }
}

void workspace_load(workspace* workspace, strv file_path)
{	
	// printf("loading %.*s into workspace(\"%.*s\")\n", str_fmt(file_path),  str_fmt(&workspace->name));
	workspace->current_directory = get_file_directory(workspace, file_path);

	da_append(
		&workspace->arena, 
		&workspace->loaded_modules, 
		(module) {
			.name = str_from_view(&workspace->arena, file_path)
		}
	);
	
	module* current_module = da_last(&workspace->loaded_modules);
	file* file = file_load(&workspace->arena, file_path);

	if(!file)
	{
		printf("%.*s file was not found\n", strv_fmt(&file_path));
		assert(0);
	}

	lexer lexer = {
		.arena = &workspace->arena, 
		.file = file,
		.out = &current_module->tokens
	};

	lexer_lex(&lexer);

	parser parser = {
		.arena = &workspace->arena,
		.tokens = &current_module->tokens
	};

	current_module->file_node = parse(workspace, &parser);
}

void workspace_load_queue(workspace* workspace)
{	
	load_queue* old_queue = workspace->load_queue;
	workspace->load_queue = arena_ppush(&workspace->arena, load_queue);

	da_foreach(old_queue)
	{		
		str abs_path = get_absolute_path(workspace, strv_from_str(old_queue->it));
		strv abs_view = strv_from_str(&abs_path);

		if(!workspace_lookup_module(workspace, abs_view)) { workspace_load(workspace, abs_view); }
		else { printf("%.*s is already loaded\n", str_fmt(&abs_path)); }
	}
}

module* workspace_lookup_module(workspace* workspace, strv name)
{
	modules_da* modules = &workspace->loaded_modules;

	da_foreach(modules)
	{	
		if(strncmp(modules->it->name.data, name.data, name.len) == 0) { return modules->it; }
	};

	return NULL;
}