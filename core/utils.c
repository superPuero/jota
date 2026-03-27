#include "utils.h"

jo_str_view_t jo_str_view_from(const char* data, jo_uz len)
{
	return (jo_str_view_t){.data = data, .len = len};
}

jo_str_view_t jo_str_view_from_cstr(const char* data)
{
	return jo_str_view_from(data, strlen(data));
}