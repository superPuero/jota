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
	arena* arena;
	file* file;
	token_ada* out;
	// ------------------------

	u32 line_counter;

	char* token_start;
	u32 token_len;

	u32 column_counter;
	u32 current;

	bool8 done;
} lexer;


void dump_tokens(token_ada* tokens);
void lexer_push_content(lexer* lexer);
bool8 lexer_content_is(lexer* lexer, const char* keyword);
void lexer_reset_content(lexer*lexer);
void lexer_newline(lexer*lexer);
void lexer_lex(lexer* lexer);

#endif
