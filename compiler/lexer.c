#include "lexer.h"
#include <ctype.h>
#include <stdio.h>


const jo_tok_keyword_entry jo_keyword_map[] = {
	#define X(tok) {#tok, sizeof(#tok) - 1, jo_tok_##tok},
	jo_tok_keyword_list
	#undef x
};

jo_token_basic_entry jo_token_map[] =
{
	// multi-char tokens first
	{"<<=", 3, jo_tok_shift_left_equals},
	{">>=", 3, jo_tok_shift_right_equals},
	{"..", 2, jo_tok_double_dot},
	{"==", 2, jo_tok_double_equals},
	{"!=", 2, jo_tok_not_equals},
	{"<=", 2, jo_tok_less_equals},
	{">=", 2, jo_tok_greater_equals},
	{"&&", 2, jo_tok_logical_and},
	{"||", 2, jo_tok_logical_or},
	{"<<", 2, jo_tok_shift_left},
	{">>", 2, jo_tok_shift_right},
	{"->", 2, jo_tok_arrow},
	{"=>", 2, jo_tok_fat_arrow},
	{"+=", 2, jo_tok_plus_equals},
	{"-=", 2, jo_tok_minus_equals},
	{"*=", 2, jo_tok_star_equals},
	{"/=", 2, jo_tok_slash_equals},
	{"%=", 2, jo_tok_modulo_equals},
	{"&=", 2, jo_tok_bitwise_and_equals},
	{"|=", 2, jo_tok_bitwise_or_equals},
	{"^=", 2, jo_tok_bitwise_xor_equals},
	{"::", 2, jo_tok_bridge},
	{":=", 2, jo_tok_walrus},
	{"(", 1, jo_tok_open_parenthesis},
	{")", 1, jo_tok_close_parenthesis},
	{"{", 1, jo_tok_open_curly_bracket},
	{"}", 1, jo_tok_close_curly_bracket},
	{"[", 1, jo_tok_open_square_bracket},
	{"]", 1, jo_tok_close_square_bracket},
	{"<", 1, jo_tok_open_angle_bracket},
	{">", 1, jo_tok_close_angle_bracket},
	{"+", 1, jo_tok_plus},
	{"-", 1, jo_tok_minus},
	{"*", 1, jo_tok_star},
	{"/", 1, jo_tok_slash},
	{"%", 1, jo_tok_modulo},
	{"=", 1, jo_tok_equals},
	{"!", 1, jo_tok_exclamation_mark},
	{"^", 1, jo_tok_caret},
	{"~", 1, jo_tok_tilde},
	{",", 1, jo_tok_comma},
	{".", 1, jo_tok_dot},
	{"&", 1, jo_tok_ampersand},
	{";", 1, jo_tok_semicolon},
	{":", 1, jo_tok_colon},
	{"#", 1, jo_tok_hash},
	{"@", 1, jo_tok_at},
	{"$", 1, jo_tok_dollar},
};

void jo_dump_tokens(jo_token_ada* tokens)
{
	jo_ada_foreach(tokens)
	{
		switch (tokens->it->type)
		{
		case jo_tok_identifier:
		case jo_tok_literal_integer:
		case jo_tok_literal_fp:
			printf("line: %u colum: %u type: %u %s (%.*s)\n", tokens->it->line, tokens->it->column, tokens->it->type, jo_tok_to_string(tokens->it->type), tokens->it->content_len, tokens->it->content);
			break;

		default:
			printf("line: %u colum: %u type: %u %s\n", tokens->it->line,  tokens->it->column, tokens->it->type, jo_tok_to_string(tokens->it->type));
			break;
		}
	}
}

char jo_lexer_peek(jo_lexer* lexer, jo_u32 offset)
{
	return lexer->file->data[lexer->current + offset];
}

char jo_lexer_peek_next(jo_lexer* lexer)
{
	return jo_lexer_peek(lexer, 0);
}

char jo_lexer_current(jo_lexer* lexer)
{
	return lexer->file->data[lexer->current];
}

void jo_lexer_advance(jo_lexer* lexer, jo_u32 by)
{
	lexer->column_counter += by;
	lexer->current += by;
	lexer->token_len += by;
}

void jo_lexer_advance_one(jo_lexer* lexer)
{
	jo_lexer_advance(lexer, 1);
}

void jo_lexer_reset_token(jo_lexer* lexer)
{
	lexer->token_len = 0;
}

void jo_lexer_make_token(jo_lexer* lexer, jo_tok type)
{
	jo_token tok = {0};		
	tok.type = type;
	tok.column = lexer->column_counter;
	tok.line = lexer->line_counter;
	tok.content = lexer->token_start;
	tok.content_len = lexer->token_len;

	jo_lexer_reset_token(lexer);

	jo_ada_append(lexer->arena, lexer->out, tok);
}

void jo_lexer_newline(jo_lexer* lexer)
{
	lexer->line_counter++;
	lexer->column_counter = 0;
}

void jo_lexer_skip_whitespace_and_comments(jo_lexer* lexer)
{
    while (true)
    {
        char c = lexer->file->data[lexer->current];
        char next_c = lexer->file->data[lexer->current + 1];

        if (c == ' ' || c == '\t' || c == '\r')
        {
            lexer->current++;
            lexer->column_counter++;
        }
        else if (c == '\n')
        {
            lexer->current++;
            lexer->line_counter++;
            lexer->column_counter = 0;
        }
        else if (c == '/' && next_c == '/')
        {
            lexer->current += 2;

            while (lexer->file->data[lexer->current] != '\n' && lexer->file->data[lexer->current] != '\0')
            {
                lexer->current++;
            }
        }
        else if (c == '/' && next_c == '*')
        {
            lexer->current += 2;
            lexer->column_counter += 2;
            int comment_depth = 1;

            while (comment_depth > 0 && lexer->file->data[lexer->current] != '\0')
            {
                if (lexer->file->data[lexer->current] == '\n')
                {
                    lexer->line_counter++;
                    lexer->column_counter = 0;
                }
                else if (lexer->file->data[lexer->current] == '/' && lexer->file->data[lexer->current + 1] == '*')
                {
                    comment_depth++;
                    lexer->current++;
                }
                else if (lexer->file->data[lexer->current] == '*' && lexer->file->data[lexer->current + 1] == '/')
                {
                    comment_depth--;
                    lexer->current++;
                }

                lexer->current++;
                lexer->column_counter++;
            }
        }
        else
        {
            break;
        }
    }
}

bool jo_lexer_match_consume(jo_lexer* lexer, const char* str, jo_u32 len)
{
	if(strncmp(lexer->token_start, str, len) == 0)
	{
		jo_lexer_advance(lexer, len);
		return true;
	}

	return false;
}

bool jo_lexer_match_content_not_consume(jo_lexer* lexer, const char* str, jo_u32 len)
{
	if(strncmp(lexer->token_start, str, len) == 0)
	{
		return true;
	}

	return false;
}


bool jo_lexer_try_match_make_token(jo_lexer* lexer, const char* str, jo_u32 len, jo_tok type)
{
	if(jo_lexer_match_consume(lexer, str, len))  { jo_lexer_make_token(lexer, type); return true; }
	else { return false; }
}

bool jo_lexer_try_match_content_make_token(jo_lexer* lexer, const char* str, jo_u32 len, jo_tok type)
{
	if(jo_lexer_match_content_not_consume(lexer, str, len))  { jo_lexer_make_token(lexer, type); return true; }
	else { return false; }
}


void jo_lexer_parse_identifier(jo_lexer* lexer)
{
	while(isalpha(jo_lexer_current(lexer))  || isdigit(jo_lexer_current(lexer)) 	|| jo_lexer_current(lexer) == '_')
	{
		jo_lexer_advance_one(lexer);
	}

	for(jo_uz i = 0; i < sizeof(jo_keyword_map)/sizeof(jo_keyword_map[0]); i++)
	{
		if(lexer->token_len == jo_keyword_map[i].len && jo_lexer_try_match_content_make_token(lexer, jo_keyword_map[i].identifier,  jo_keyword_map[i].len, jo_keyword_map[i].type)) return;
	}

	jo_lexer_make_token(lexer, jo_tok_identifier);
}

void jo_lexer_parse_number(jo_lexer* lexer)
{
	bool has_dot = false;

	while(isdigit(jo_lexer_current(lexer)) || jo_lexer_current(lexer) == '.')
	{
		if(jo_lexer_current(lexer) == '.')
		{
			if(has_dot)
			{
				break;
			}
			else
			{
				has_dot = true;
			}
		}

	 	jo_lexer_advance_one(lexer);
	}

	if(has_dot)
	{
		jo_lexer_make_token(lexer, jo_tok_literal_fp);
	}
	else
	{
		jo_lexer_make_token(lexer, jo_tok_literal_integer);
	}
}

void jo_lexer_parse_string(jo_lexer* lexer)
{
	jo_lexer_advance_one(lexer);

	char curr = jo_lexer_current(lexer);

	lexer->token_start++;

	while(true)
	{				
		//@todo: this is very brittle and straight up wrong
		curr = jo_lexer_current(lexer);	
		if(curr == '"') 
		{
			jo_lexer_advance_one(lexer);
			break; 
		}		
		jo_lexer_advance_one(lexer);
	}
	
	lexer->token_len--;
	lexer->token_len--;

	jo_lexer_make_token(lexer, jo_tok_literal_string);
}

void jo_lexer_parse_next(jo_lexer* lexer)
{
	jo_lexer_skip_whitespace_and_comments(lexer);

	if(lexer->current == lexer->file->len || lexer->current == lexer->file->len + 1) { lexer->done = true; return; }

	lexer->token_start = lexer->file->data + lexer->current;

	char c = jo_lexer_current(lexer);

	if(isalpha(c) || c == '_') { jo_lexer_parse_identifier(lexer); return;}
	if(isdigit(c)) { jo_lexer_parse_number(lexer); return; }
	if(c == '"') { jo_lexer_parse_string(lexer); return; }
	
	for(jo_uz i = 0; i < sizeof(jo_token_map)/sizeof(jo_token_map[0]); ++i)
	{
		if(jo_lexer_try_match_make_token(lexer, jo_token_map[i].content, jo_token_map[i].len, jo_token_map[i].type)){return;}
	}

	jo_lexer_advance_one(lexer);
}

void jo_lexer_lex(jo_lexer* lexer)
{
	if(!lexer->arena)
	{
		printf("lexer must be provide with memory arena\n");
		assert(0);
	}
	if(!lexer->file)
	{
		printf("lexer must be provide with file\n");
		assert(0);
	}
	if(!lexer->out)
	{
		printf("lexer must be provide with out token ada\n");
		assert(0);
	}


	jo_lexer_newline(lexer);

	while(!lexer->done)
	{
		jo_lexer_parse_next(lexer);
	}

	jo_lexer_make_token(lexer, jo_tok_eof);
}

