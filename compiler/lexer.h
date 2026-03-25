#ifndef jota_lexer
#define jota_lexer

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../core/core.h"
#include "ast_node.h"
#include "token.h"

typedef struct 
{
	jo_arena_t* arena;

	jo_token_dyn_array_t tokens;

	char* data;
	jo_u32 data_size;
	
	jo_u32 line_counter;

	jo_string filename;

	char* token_start;
	jo_u32 token_len;

	jo_u32 column_counter;
	jo_u32 current;

	bool done;
} jo_lexer_t;

void jo_lexer_proceed(jo_lexer_t *lexer, FILE *file);
void jo_lexer_append_token(jo_lexer_t *lexer, jo_token_dyn_array_t *tokens, jo_token_type_t type, bool with_content);
void jo_lexer_push_content(jo_lexer_t *lexer);
bool jo_lexer_content_is(jo_lexer_t *lexer, const char *keyword);
void jo_lexer_reset_content(jo_lexer_t *lexer);
void jo_handle_content_break(jo_lexer_t *lexer, jo_token_dyn_array_t *tokens);
void jo_lexer_newline(jo_lexer_t *lexer);
jo_success jo_lex_file(jo_lexer_t* lexer, const char *filename);

#endif
