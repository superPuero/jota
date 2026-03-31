#include "compile_options.h"	

jo_compile_options jo_compile_options_parse_from_args(int argc, char** argv)
{
	jo_compile_options opt = {0};
	opt.success = true;

	if(argc < 2)
	{
		fprintf(stderr, "provide soruce file to parse\n");
		opt.success = false;
		return opt;
	}

	for(jo_i32 i = 1; i < argc; i++)
	{
		jo_bool found = false;

		#define XY(e, help_info)\
		if(strcmp(argv[i], "-"#e) == 0)\
		{\
			opt.e = true;\
			found = true;\
		}
		jo_compile_options_list
		#undef XY

		if(!found)
		{
			if(!opt.file_provided)
			{								
				opt.file = jo_str_view_from(argv[i]);	
				opt.file_provided = true;			
			}
			else
			{
				printf("unrecognized argument: %s\n", argv[i]);
				opt.success = false;
				break;
			}
		}
	}

	return opt;
}
