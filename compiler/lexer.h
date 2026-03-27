#ifndef jota_lexer
#define jota_lexer

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../core/core.h"
#include "ast_node.h"
#include "token.h"

jo_ada_declare(jo_token , jo_token_ada);

typedef struct 
{
	jo_arena * arena;

	jo_token_ada tokens;

	char* data;
	jo_u32 data_size;
	
	jo_u32 line_counter;

	jo_astr filename;

	char* token_start;
	jo_u32 token_len;

	jo_u32 column_counter;
	jo_u32 current;

	bool done;
} jo_lexer;

void jo_lexer_proceed(jo_lexer* lexer, FILE *file);
void jo_lexer_push_content(jo_lexer* lexer);
bool jo_lexer_content_is(jo_lexer* lexer, const char *keyword);
void jo_lexer_reset_content(jo_lexer*lexer);
void jo_lexer_newline(jo_lexer*lexer);
jo_success jo_lex_file(jo_lexer* lexer, const char *filename);

#endif
