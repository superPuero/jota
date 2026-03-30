#include "compile_options.h"	

jo_compile_options jo_compile_options_parse_from_args(int argc, char** argv)
{
	jo_compile_options opt = {0};
	opt.success = true;

	if(argc < 2)
	{
		printf("prodive soruce file to parse\n");
		opt.success = false;
		return opt;
	}

	opt.filepath = jo_str_view_from(argv[1]);

	for(jo_i32 i = 2; i < argc; i++)
	{
		if(strcmp(argv[i], "-t") == 0)
		{
			opt.tokens = true;
		}
		else if(strcmp(argv[i], "-dt") == 0)
		{
			opt.tokens = true;
			opt.tokens_dump = true;
		}
		else if(strcmp(argv[i], "-ast") == 0)
		{
			opt.tokens = true;
			opt.ast = true;
		}
		else if(strcmp(argv[i], "-dast") == 0)
		{
			opt.tokens = true;
			opt.ast = true;
			opt.ast_dump = true;
		}
		else if(strcmp(argv[i], "-sema") == 0)
		{		
			opt.tokens = true;
			opt.ast = true;
			opt.sema = true;
		}
		else if(strcmp(argv[i], "-bc") == 0)
		{
			opt.tokens = true;
			opt.ast = true;
			opt.sema = true;
			opt.bytecode = true;
		}
		else if(strcmp(argv[i], "-dbc") == 0)
		{
			opt.tokens = true;
			opt.ast = true;
			opt.sema = true;
			opt.bytecode = true;
			opt.bytecode_dump = true;
		}
		else if(strcmp(argv[i], "-i") == 0)
		{
			opt.tokens = true;
			opt.ast = true;
			opt.sema = true;
			opt.bytecode = true;
			opt.interp = true;
		}
		else if(strcmp(argv[i], "-time") == 0)
		{
			opt.time = true;
		}
		else
		{
			printf("unrecognized argument: %s", argv[i]);
			opt.success = false;
			break;
		}
	}

	return opt;
}
