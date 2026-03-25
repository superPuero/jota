#include "compile_options.h"	

jo_compile_options jo_compie_options_parse_from_args(int argc, char** argv)
{
	jo_compile_options opt = {0};
	opt.success = true;

	if(argc < 2)
	{
		printf("prodive soruce file to parse");
		opt.success = false;
		return opt;
	}

	for(jo_u32 i = 2; i < argc; i++)
	{
		if(strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "-h") == 0)
		{
		}
		if(strcmp(argv[i], "-ast") == 0)
		{
			opt.ast = true;
		}
		else if(strcmp(argv[i], "-tokens") == 0 || strcmp(argv[i], "-t") == 0)
		{
			opt.tokens = true;
		}
		else if(strcmp(argv[i], "-sema") == 0)
		{
			opt.sema = true;
		}
		else if(strcmp(argv[i], "-test") == 0)
		{
		}
		else if(strcmp(argv[i], "-bytecode") == 0 || strcmp(argv[i], "-bc") == 0)
		{\
			opt.bytecode = true;
		}
		else if(strcmp(argv[i], "-interp") == 0 || strcmp(argv[i], "-i") == 0)
		{
			opt.interp = true;
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
