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
	// --- must be provided ---
	jo_arena* arena;
	jo_file* file;
	jo_token_ada* out;
	// ------------------------

	jo_u32 line_counter;

	char* token_start;
	jo_u32 token_len;

	jo_u32 column_counter;
	jo_u32 current;

	bool done;
} jo_lexer;


void jo_dump_tokens(jo_token_ada* tokens);
void jo_lexer_push_content(jo_lexer* lexer);
bool jo_lexer_content_is(jo_lexer* lexer, const char* keyword);
void jo_lexer_reset_content(jo_lexer*lexer);
void jo_lexer_newline(jo_lexer*lexer);
void jo_lexer_lex(jo_lexer* lexer);

#endif
