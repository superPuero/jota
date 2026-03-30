#include "utils.h"
#include "platform_detect.h"

jo_str_view jo_str_view_make(const char* data, jo_uz len)
{
	return (jo_str_view){.data = data, .len = len};
}

jo_str_view jo_str_view_from(const char* data)
{
	return jo_str_view_make(data, strlen(data));
}
