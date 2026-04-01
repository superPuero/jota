#include "utils.h"
#include "platform_detect.h"

str_view str_view_make(const char* data, uz len)
{
	return (str_view){.data = data, .len = len};
}

str_view str_view_from(const char* data)
{
	return str_view_make(data, strlen(data));
}
