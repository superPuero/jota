#include "string.h"
#include <stdio.h>

bool jo_str_ends_with(const char* str, const char* 	postfix)
{
    if (!str || !postfix) 
	{
        return false;
    }

    size_t str_len = strlen(str);
    size_t postfix_len = strlen(postfix);

    if (postfix_len > str_len) 
	{
        return false;
    }

    const char *start_of_suffix_in_str = str + (str_len - postfix_len);
    
    return strncmp(start_of_suffix_in_str, postfix, postfix_len) == 0;
}


jo_string jo_string_from(const char* chars)
{
	jo_string str = {0};

	jo_string_append(&str, chars);

	return str;
}

jo_string jo_string_from_n(const char* chars, jo_u32 len)
{
	jo_string str = {0};
	for(jo_u32 i = 0; i < len; i++)
	{
		jo_string_append_ch(&str, chars[i]);
	}
	
	return str;
}

// @HACK: there are better ways, but i dont care for now
void jo_string_append_ch(jo_string* str, char ch)
{
	char b[2] = {ch, '\0'};

	jo_string_append(str, b);
}

void jo_string_append(jo_string* str, const char* chars)
{
	jo_u32 len = strlen(chars);

	if(!str->size)
	{
		str->capacity = len + 1;
		str->data = malloc(str->capacity);	
		memcpy(str->data, chars, len);
		str->data[len] = '\0';
		str->size = len;
		return;	
	}

	if(str->size + len + 1 > str->capacity)
	{
		do
		{
			str->capacity = str->capacity * jo_string_growth_coef;
		}while(str->capacity < str->size + len + 1);

		str->data = realloc(str->data, str->capacity);
	}

	memcpy(str->data + str->size, chars, len);
	str->size += len;
	str->data[str->size] = '\0';
}

void jo_string_free(jo_string* string)
{
	if (string->data) 
	{
        free(string->data);
        string->data = NULL;
        string->size = 0;
        string->capacity = 0;
    }
}