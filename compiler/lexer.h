#ifndef jota_lexer
#define jota_lexer

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../core/core.h"

#include "token.h"

typedef struct
{	
	FILE* stream;
	jo_token_dyn_array_t tokens;
	jo_u32 line_counter;
	jo_u32 column_counter;
	int current_char;
	char current_content[jo_token_content_max_length];
	jo_u32 current_content_len;
	bool done;
} jo_lexer_t;

void jo_lexer_proceed(jo_lexer_t* lexer, FILE* file);

void jo_lexer_append_token(jo_lexer_t* lexer, jo_token_dyn_array_t* tokens, jo_token_type_t type, bool with_content);

void jo_lexer_push_content(jo_lexer_t* lexer);

bool jo_lexer_content_is(jo_lexer_t* lexer, const char* keyword);

void jo_lexer_reset_content(jo_lexer_t* lexer);

void jo_handle_content_break(jo_lexer_t* lexer, jo_token_dyn_array_t* tokens);

void jo_lexer_newline(jo_lexer_t* lexer);

jo_token_dyn_array_t jo_lex_file(const char* filename);

#endif