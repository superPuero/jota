#include "compile_options.h"	

compile_options compile_options_parse_from_args(int argc, char** argv)
{
	compile_options opt = {0};
	opt.success = true;

	if(argc < 2)
	{
		fprintf(stderr, "provide soruce file to parse\n");
		opt.success = false;
		return opt;
	}

	for(i32 i = 1; i < argc; i++)
	{
		bool8 found = false;

		#define XY(e, help_info)\
		if(strcmp(argv[i], "-"#e) == 0)\
		{\
			opt.e = true;\
			found = true;\
		}
		compile_options_list
		#undef XY

		if(!found)
		{
			if(!opt.file_provided)
			{								
				opt.file = strv_from_cstr(argv[i]);	
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
