#include "lexer.h"
#include <ctype.h>
#include <stdio.h>


jo_token_keyword_entry jo_keyword_map[] = {
	jo_token_make_keyword_entry(type, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(i8, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(u8, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(i16, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(u16, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(i32, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(u32, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(i64, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(u64, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(f32, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(f64, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(bool, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(void, jo_token_kind_type_primitive),
	jo_token_make_keyword_entry(true, jo_token_kind_literal),
	jo_token_make_keyword_entry(false, jo_token_kind_literal),
	jo_token_make_keyword_entry(as, jo_token_kind_none),
	jo_token_make_keyword_entry(return, jo_token_kind_none),
	jo_token_make_keyword_entry(struct, jo_token_kind_none),
	jo_token_make_keyword_entry(fn, jo_token_kind_none),
	jo_token_make_keyword_entry(let, jo_token_kind_none),
	jo_token_make_keyword_entry(if, jo_token_kind_none),
	jo_token_make_keyword_entry(else, jo_token_kind_none),
	jo_token_make_keyword_entry(match, jo_token_kind_none),
	jo_token_make_keyword_entry(nil, jo_token_kind_literal),
	jo_token_make_keyword_entry(enum, jo_token_kind_none),
	jo_token_make_keyword_entry(static, jo_token_kind_none),
	jo_token_make_keyword_entry(import, jo_token_kind_none),
	jo_token_make_keyword_entry(module, jo_token_kind_none),
	jo_token_make_keyword_entry(defer, jo_token_kind_none),
	jo_token_make_keyword_entry(for, jo_token_kind_none),
	jo_token_make_keyword_entry(in, jo_token_kind_none),
	jo_token_make_keyword_entry(break, jo_token_kind_none),
	jo_token_make_keyword_entry(continue, jo_token_kind_none),
	jo_token_make_keyword_entry(namespace, jo_token_kind_none),
	jo_token_make_keyword_entry(load, jo_token_kind_none),
	jo_token_make_keyword_entry(intrinsic, jo_token_kind_none),
};

char jo_lexer_peek(jo_lexer_t* lexer, jo_u32 offset)
{
	return lexer->data[lexer->current + offset];
}

char jo_lexer_peek_next(jo_lexer_t* lexer)
{
	return jo_lexer_peek(lexer, 0);
}

char jo_lexer_current(jo_lexer_t* lexer)
{
	return lexer->data[lexer->current];
}

void jo_lexer_advance(jo_lexer_t* lexer, jo_u32 by)
{
	lexer->column_counter += by;
	lexer->current += by;
	lexer->token_len += by;
}

void jo_lexer_advance_one(jo_lexer_t* lexer)
{
	jo_lexer_advance(lexer, 1);
}

void jo_lexer_reset_token(jo_lexer_t* lexer)
{
	lexer->token_len = 0;
}

void jo_lexer_make_token(jo_lexer_t* lexer, jo_token_type_t type, jo_token_kind kind)
{
	jo_token_t tok = {0};
	tok.type = type;
	tok.kind = kind;
	tok.column = lexer->column_counter;
	tok.line = lexer->line_counter;
	tok.content = lexer->token_start;
	tok.content_len = lexer->token_len;

	jo_lexer_reset_token(lexer);

	jo_ada_append(lexer->arena, &lexer->tokens, tok);
}

void jo_lexer_newline(jo_lexer_t* lexer)
{
	lexer->line_counter++;
	lexer->column_counter = 0;
}

void jo_lexer_skip_whitespace_and_comments(jo_lexer_t* lexer)
{
    while (true)
    {
        char c = lexer->data[lexer->current];
        char next_c = lexer->data[lexer->current + 1];

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

            while (lexer->data[lexer->current] != '\n' && lexer->data[lexer->current] != '\0')
            {
                lexer->current++;
            }
        }
        else if (c == '/' && next_c == '*')
        {
            lexer->current += 2;
            lexer->column_counter += 2;
            int comment_depth = 1;

            while (comment_depth > 0 && lexer->data[lexer->current] != '\0')
            {
                if (lexer->data[lexer->current] == '\n')
                {
                    lexer->line_counter++;
                    lexer->column_counter = 0;
                }
                else if (lexer->data[lexer->current] == '/' && lexer->data[lexer->current + 1] == '*')
                {
                    comment_depth++;
                    lexer->current++;
                }
                else if (lexer->data[lexer->current] == '*' && lexer->data[lexer->current + 1] == '/')
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

bool jo_lexer_match_consume(jo_lexer_t* lexer, const char* str)
{
	jo_u32 len = strlen(str);

	if(strncmp(lexer->token_start, str, len) == 0)
	{
		jo_lexer_advance(lexer, len);
		return true;
	}

	return false;
}

bool jo_lexer_match_content_not_consume(jo_lexer_t* lexer, const char* str)
{
	jo_u32 len = strlen(str);
	if(len < lexer->token_len) return false;

	if(strncmp(lexer->token_start, str, len) == 0)
	{
		return true;
	}

	return false;
}


bool jo_lexer_try_match_make_token(jo_lexer_t* lexer, const char* str, jo_token_type_t type, jo_token_kind kind)
{
	if(jo_lexer_match_consume(lexer, str))  { jo_lexer_make_token(lexer, type, kind); return true; }
	else { return false; }
}

bool jo_lexer_try_match_content_make_token(jo_lexer_t* lexer, const char* str, jo_token_type_t type, jo_token_kind kind)
{
	if(jo_lexer_match_content_not_consume(lexer, str))  { jo_lexer_make_token(lexer, type, kind); return true; }
	else { return false; }
}


void jo_lexer_parse_identifier(jo_lexer_t* lexer)
{
	while(isalpha(jo_lexer_current(lexer))  || isdigit(jo_lexer_current(lexer)) 	|| jo_lexer_current(lexer) == '_')
	{
		jo_lexer_advance_one(lexer);
	}

	for(jo_uz i = 0; i < sizeof(jo_keyword_map)/sizeof(jo_keyword_map[0]); i++)
	{
		if(jo_lexer_try_match_content_make_token(lexer, jo_keyword_map[i].identifier, jo_keyword_map[i].type, jo_keyword_map[i].kind)) return;
	}

	jo_lexer_make_token(lexer, jo_token_identifier, jo_token_kind_none);
}

void jo_lexer_parse_number(jo_lexer_t* lexer)
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
		jo_lexer_make_token(lexer, jo_token_literal_fp, jo_token_kind_literal);
	}
	else
	{
		jo_lexer_make_token(lexer, jo_token_literal_integer, jo_token_kind_literal);
	}
}

void jo_lexer_parse_string(jo_lexer_t* lexer)
{
	jo_lexer_advance_one(lexer);

	char curr = jo_lexer_current(lexer);

	lexer->token_start++;

	while(true)
	{
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

	jo_lexer_make_token(lexer, jo_token_literal_string, jo_token_kind_literal);
}

void jo_lexer_parse_next(jo_lexer_t* lexer)
{
	jo_lexer_skip_whitespace_and_comments(lexer);

	lexer->token_start = lexer->data + lexer->current;

	char c = jo_lexer_current(lexer);
	if(isalpha(c) || c == '_') { jo_lexer_parse_identifier(lexer); return;}
	if(isdigit(c)) { jo_lexer_parse_number(lexer); return; }
	if(c == '"') { jo_lexer_parse_string(lexer); return; }
	
	// multi-char tokens first
	if (jo_lexer_try_match_make_token(lexer, "<<=", jo_token_shift_left_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, ">>=", jo_token_shift_right_equals, jo_token_kind_none)) return;

	if (jo_lexer_try_match_make_token(lexer, "..", jo_token_double_dot, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "==", jo_token_double_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "!=", jo_token_not_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "<=", jo_token_less_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, ">=", jo_token_greater_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "&&", jo_token_logical_and, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "||", jo_token_logical_or, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "<<", jo_token_shift_left, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, ">>", jo_token_shift_right, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "->", jo_token_arrow, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, "=>", jo_token_fat_arrow, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, "+=", jo_token_plus_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "-=", jo_token_minus_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "*=", jo_token_star_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "/=", jo_token_slash_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "%=", jo_token_modulo_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "&=", jo_token_bitwise_and_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "|=", jo_token_bitwise_or_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "^=", jo_token_bitwise_xor_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "::", jo_token_bridge, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, ":=", jo_token_walrus, jo_token_kind_none)) return;


	if (jo_lexer_try_match_make_token(lexer, "(", jo_token_open_parenthesis, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, ")", jo_token_close_parenthesis, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, "{", jo_token_open_curly_bracket, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, "}", jo_token_close_curly_bracket, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, "[", jo_token_open_square_bracket, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, "]", jo_token_close_square_bracket, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, "<", jo_token_open_angle_bracket, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, ">", jo_token_close_angle_bracket, jo_token_kind_type_operator)) return;

	if (jo_lexer_try_match_make_token(lexer, "+", jo_token_plus, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "-", jo_token_minus, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "*", jo_token_star, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "/", jo_token_slash, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "%", jo_token_modulo, jo_token_kind_type_operator)) return;

	if (jo_lexer_try_match_make_token(lexer, "=", jo_token_equals, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "!", jo_token_exclamation_mark, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "^", jo_token_caret, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, "~", jo_token_tilde, jo_token_kind_type_operator)) return;
	if (jo_lexer_try_match_make_token(lexer, ",", jo_token_comma, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, ".", jo_token_dot, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, "&", jo_token_ampersand, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, ";", jo_token_semicolon, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, ":", jo_token_colon, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, "#", jo_token_hash, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, "@", jo_token_at, jo_token_kind_none)) return;
	if (jo_lexer_try_match_make_token(lexer, "$", jo_token_dollar, jo_token_kind_none)) return;

	if(lexer->current == lexer->data_size) { lexer->done = true; return; }

	jo_lexer_advance_one(lexer);
}

jo_success jo_lexer_open_and_load(jo_lexer_t* lexer, const char* filename)
{
	lexer->filename = jo_astr_from(lexer->arena, filename);
	FILE* file = fopen(filename, "rb");

	if (!file)
    {
        printf("error: could not open file '%s'\n", filename);
		return false; 
    }

	fseek(file, 0, SEEK_END);
	lexer->data_size =  ftell(file);
	rewind(file);
	lexer->data = jo_arena_alloc(lexer->arena, lexer->data_size + 1);

	fread(lexer->data, 1, lexer->data_size, file);
	lexer->data[lexer->data_size] = '\0';
	fclose(file);

	return true;
}

void jo_lexer_close(jo_lexer_t* lexer)
{
	free(lexer->data);
}

void jo_lexer_lex(jo_lexer_t* lexer)
{
	jo_lexer_newline(lexer);

	while(!lexer->done)
	{
		jo_lexer_parse_next(lexer);
	}

	jo_lexer_make_token(lexer, jo_token_eof, jo_token_kind_none);
}

jo_success jo_lex_file(jo_lexer_t* lexer, const char* filename)
{
	if(!lexer->arena)
	{
		printf("lexer must be provide with memory arena\n");
		return false;
	}

	jo_success succ = true;  

	succ = jo_lexer_open_and_load(lexer, filename);
	if(!succ)
	{
		printf("file loading error\n");
		return false;
	}

	jo_lexer_lex(lexer);

	return true;
}
