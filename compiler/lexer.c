#include "lexer.h"
#include <ctype.h>
#include <stdio.h>


const tok_keyword_entry keyword_map[] = {
	#define X(tok) {#tok, sizeof(#tok) - 1, tok_##tok},
	tok_keyword_list
	#undef x
};

token_basic_entry token_map[] =
{
	// multi-char tokens first
	{"<<=", 3, tok_shift_left_equals},
	{">>=", 3, tok_shift_right_equals},
	{"..", 2, tok_double_dot},
	{"==", 2, tok_double_equals},
	{"!=", 2, tok_not_equals},
	{"<=", 2, tok_less_equals},
	{">=", 2, tok_greater_equals},
	{"&&", 2, tok_logical_and},
	{"||", 2, tok_logical_or},
	{"<<", 2, tok_shift_left},
	{">>", 2, tok_shift_right},
	{"->", 2, tok_arrow},
	{"=>", 2, tok_fat_arrow},
	{"+=", 2, tok_plus_equals},
	{"-=", 2, tok_minus_equals},
	{"*=", 2, tok_star_equals},
	{"/=", 2, tok_slash_equals},
	{"%=", 2, tok_modulo_equals},
	{"&=", 2, tok_bitwise_and_equals},
	{"|=", 2, tok_bitwise_or_equals},
	{"^=", 2, tok_bitwise_xor_equals},
	{"::", 2, tok_bridge},
	{":=", 2, tok_walrus},
	{"(", 1, tok_open_parenthesis},
	{")", 1, tok_close_parenthesis},
	{"{", 1, tok_open_curly_bracket},
	{"}", 1, tok_close_curly_bracket},
	{"[", 1, tok_open_square_bracket},
	{"]", 1, tok_close_square_bracket},
	{"<", 1, tok_open_angle_bracket},
	{">", 1, tok_close_angle_bracket},
	{"+", 1, tok_plus},
	{"-", 1, tok_minus},
	{"*", 1, tok_star},
	{"/", 1, tok_slash},
	{"%", 1, tok_modulo},
	{"=", 1, tok_equals},
	{"!", 1, tok_exclamation_mark},
	{"^", 1, tok_caret},
	{"~", 1, tok_tilde},
	{",", 1, tok_comma},
	{".", 1, tok_dot},
	{"&", 1, tok_ampersand},
	{";", 1, tok_semicolon},
	{":", 1, tok_colon},
	{"#", 1, tok_hash},
	{"@", 1, tok_at},
	{"$", 1, tok_dollar},
};

void dump_tokens(token_ada* tokens)
{
	ada_foreach(tokens)
	{
		switch (tokens->it->type)
		{
		case tok_identifier:
		case tok_literal_integer:
		case tok_literal_fp:
			printf("line: %u colum: %u type: %u %s (%.*s)\n", tokens->it->line, tokens->it->column, tokens->it->type, tok_to_string(tokens->it->type), tokens->it->content_len, tokens->it->content);
			break;

		default:
			printf("line: %u colum: %u type: %u %s\n", tokens->it->line,  tokens->it->column, tokens->it->type, tok_to_string(tokens->it->type));
			break;
		}
	}
}

char lexer_peek(lexer* lexer, u32 offset)
{
	return lexer->file->data[lexer->current + offset];
}

char lexer_peek_next(lexer* lexer)
{
	return lexer_peek(lexer, 0);
}

char lexer_current(lexer* lexer)
{
	return lexer->file->data[lexer->current];
}

void lexer_advance(lexer* lexer, u32 by)
{
	lexer->column_counter += by;
	lexer->current += by;
	lexer->token_len += by;
}

void lexer_advance_one(lexer* lexer)
{
	lexer_advance(lexer, 1);
}

void lexer_reset_token(lexer* lexer)
{
	lexer->token_len = 0;
}

void lexer_make_token(lexer* lexer, tok type)
{
	token tok = {0};		
	tok.type = type;
	tok.column = lexer->column_counter;
	tok.line = lexer->line_counter;
	tok.content = lexer->token_start;
	tok.content_len = lexer->token_len;

	lexer_reset_token(lexer);

	ada_append(lexer->arena, lexer->out, tok);
}

void lexer_newline(lexer* lexer)
{
	lexer->line_counter++;
	lexer->column_counter = 0;
}

void lexer_skip_whitespace_and_comments(lexer* lexer)
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

bool8 lexer_match_consume(lexer* lexer, const char* str, u32 len)
{
	if(strncmp(lexer->token_start, str, len) == 0)
	{
		lexer_advance(lexer, len);
		return true;
	}

	return false;
}

bool8 lexer_match_content_not_consume(lexer* lexer, const char* str, u32 len)
{
	if(strncmp(lexer->token_start, str, len) == 0)
	{
		return true;
	}

	return false;
}


bool8 lexer_try_match_make_token(lexer* lexer, const char* str, u32 len, tok type)
{
	if(lexer_match_consume(lexer, str, len))  { lexer_make_token(lexer, type); return true; }
	else { return false; }
}

bool8 lexer_try_match_content_make_token(lexer* lexer, const char* str, u32 len, tok type)
{
	if(lexer_match_content_not_consume(lexer, str, len))  { lexer_make_token(lexer, type); return true; }
	else { return false; }
}


void lexer_parse_identifier(lexer* lexer)
{
	while(isalpha(lexer_current(lexer))  || isdigit(lexer_current(lexer)) || lexer_current(lexer) == '_')
	{
		lexer_advance_one(lexer);
	}

	for(uz i = 0; i < sizeof(keyword_map)/sizeof(keyword_map[0]); i++)
	{
		if(lexer->token_len == keyword_map[i].len && lexer_try_match_content_make_token(lexer, keyword_map[i].identifier,  keyword_map[i].len, keyword_map[i].type)) return;
	}

	lexer_make_token(lexer, tok_identifier);
}

void lexer_parse_number(lexer* lexer)
{
	bool8 has_dot = false;

	while(isdigit(lexer_current(lexer)) || lexer_current(lexer) == '.')
	{
		if(lexer_current(lexer) == '.')
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

	 	lexer_advance_one(lexer);
	}

	if(has_dot)
	{
		lexer_make_token(lexer, tok_literal_fp);
	}
	else
	{
		lexer_make_token(lexer, tok_literal_integer);
	}
}

void lexer_parse_string(lexer* lexer)
{
	lexer_advance_one(lexer);

	char curr = lexer_current(lexer);

	lexer->token_start++;

	while(true)
	{				
		//@todo: this is very brittle and straight up wrong
		curr = lexer_current(lexer);	
		if(curr == '"') 
		{
			lexer_advance_one(lexer);
			break; 
		}		
		lexer_advance_one(lexer);
	}
	
	lexer->token_len--;
	lexer->token_len--;

	lexer_make_token(lexer, tok_literal_string);
}

void lexer_parse_next(lexer* lexer)
{
	lexer_skip_whitespace_and_comments(lexer);

	if(lexer->current == lexer->file->len || lexer->current == lexer->file->len + 1) { lexer->done = true; return; }

	lexer->token_start = lexer->file->data + lexer->current;

	char c = lexer_current(lexer);

	if(isalpha(c) || c == '_') { lexer_parse_identifier(lexer); return;}
	if(isdigit(c)) { lexer_parse_number(lexer); return; }
	if(c == '"') { lexer_parse_string(lexer); return; }
	
	for(uz i = 0; i < sizeof(token_map)/sizeof(token_map[0]); ++i)
	{
		if(lexer_try_match_make_token(lexer, token_map[i].content, token_map[i].len, token_map[i].type)){ return; }
	}

	lexer_advance_one(lexer);
}

void lexer_lex(lexer* lexer)
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

	lexer_newline(lexer);

	while(!lexer->done)
	{
		lexer_parse_next(lexer);
	}

	lexer_make_token(lexer, tok_eof);
}

