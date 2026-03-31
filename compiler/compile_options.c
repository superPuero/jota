#include "compile_options.h"	

jo_compile_options jo_compile_options_parse_from_args(int argc, char** argv)
{
	jo_compile_options opt = {0};
	opt.success = true;

	if(argc < 2)
	{
		printf("provide soruce file to parse\n");
		opt.success = false;
		return opt;
	}

	opt.filepath = jo_str_view_from(argv[1]);

	for(jo_i32 i = 2; i < argc; i++)
	{
		jo_bool found = false;

		#define X(e)\
		if(strcmp(argv[i], "-"#e) == 0)\
		{\
			opt.e = true;\
			found = true;\
		}
		jo_compile_options_list
		#undef X

		if(!found)
		{
			printf("unrecognized argument: %s\n", argv[i]);
			opt.success = false;
			break;
		}
	}
	
	return opt;
}
