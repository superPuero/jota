#include "lexer.h"

void jo_lexer_advance(jo_lexer_t* lexer)
{
	lexer->column_counter++;
	if(lexer->current_content_len > jo_token_content_max_length)
	{
		jo_err("token content is too big: %s\n", lexer->current_content);
	}
	lexer->current_char = fgetc(lexer->stream);
}

void jo_lexer_make_token_maybe_consume(jo_lexer_t* lexer, jo_token_type_t type, jo_token_kind kind, bool with_content)
{
	jo_token_t token;
	token.type = type;	
	token.line = lexer->line_counter;
	token.column = lexer->column_counter;
	token.kind = kind;

	if(with_content)
	{
		memcpy(token.content, lexer->current_content, lexer->current_content_len);
		token.content[lexer->current_content_len] = '\0';
		token.column -= strlen(token.content);
	}
	
	jo_lexer_reset_content(lexer);

	jo_dyn_array_append(&lexer->tokens, token);
}

void jo_lexer_push_content(jo_lexer_t* lexer)
{
	lexer->current_content[lexer->current_content_len++] = lexer->current_char;
}

bool jo_lexer_content_is(jo_lexer_t* lexer, const char* keyword)
{
	if(lexer->current_content_len < strlen(keyword)) return false;
	return strncmp(lexer->current_content, keyword, lexer->current_content_len) == 0;
}

void jo_lexer_reset_content(jo_lexer_t* lexer)
{
	memset(lexer->current_content, 0, jo_token_content_max_length);
	lexer->current_content_len = 0;
}

void jo_lexer_parse_literal_or_identifier(jo_lexer_t* lexer)
{
	if(lexer->current_content[0] == '\"' && lexer->current_content[lexer->current_content_len - 1]  == '\"')
	{
		jo_lexer_make_token_maybe_consume(lexer, jo_token_literal_string, jo_token_kind_none, true);
	}
	else if(lexer->current_content[0] >= '0' && lexer->current_content[0] <= '9')
	{
		jo_lexer_make_token_maybe_consume(lexer, jo_token_literal_integer, jo_token_kind_none, true);
	}
	else
	{
		jo_lexer_make_token_maybe_consume(lexer, jo_token_identifier, jo_token_kind_none, true);
	}
}

void jo_lexer_handle_content_break(jo_lexer_t* lexer)
{
	if(lexer->current_content_len < 1) return; 

	if(jo_lexer_content_is(lexer, "i32"))
	{
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_i32, jo_token_kind_type_primitive, false);
	}
	else if(jo_lexer_content_is(lexer, "u32"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_u32, jo_token_kind_type_primitive, false);
	}
	else if(jo_lexer_content_is(lexer, "bool"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_bool, jo_token_kind_type_primitive, false);
	}
	else if(jo_lexer_content_is(lexer, "void"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_void, jo_token_kind_type_primitive, false);
	}
	else if(jo_lexer_content_is(lexer, "true"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_true, jo_token_kind_none, false);
	}
	else if(jo_lexer_content_is(lexer, "false"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_false, jo_token_kind_none, false);
	}
	else if(jo_lexer_content_is(lexer, "struct"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_struct, jo_token_kind_none, false);
	}
	else if(jo_lexer_content_is(lexer, "return"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_return, jo_token_kind_none, false);
	}
	else if(jo_lexer_content_is(lexer, "fn"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_fn, jo_token_kind_none, false);
	}
	else if(jo_lexer_content_is(lexer, "if"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_if, jo_token_kind_none, false);
	}
	else if(jo_lexer_content_is(lexer, "else"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_else, jo_token_kind_none, false);
	}
	else if(jo_lexer_content_is(lexer, "match"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_match, jo_token_kind_none, false);
	}
	else if(jo_lexer_content_is(lexer, "nil"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_nil, jo_token_kind_none, false);
	}
	else if(jo_lexer_content_is(lexer, "static"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_static, jo_token_kind_none, false);
	}
	else if(jo_lexer_content_is(lexer, "enum"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_enum, jo_token_kind_none, false);
	}
	else if(jo_lexer_content_is(lexer, "defer"))
	{		
		jo_lexer_make_token_maybe_consume(lexer, jo_token_keyword_defer, jo_token_kind_none, false);
	}
	else
	{
		jo_lexer_parse_literal_or_identifier(lexer);
	}
}

void jo_lexer_newline(jo_lexer_t* lexer)
{
	lexer->line_counter++;
	lexer->column_counter = 0;
}

void jo_lexer_parse_next(jo_lexer_t* lexer)
{
	switch (lexer->current_char)
	{
	case '\n':
		jo_lexer_handle_content_break(lexer);				
		jo_lexer_newline(lexer);
		break;

	case ' ':
	case '\t':
		jo_lexer_handle_content_break(lexer);
		break;
	
	case '#':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_hash, jo_token_kind_none, false);			
		break;
	case ',':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_comma, jo_token_kind_none, false);			
		break;

	case '+':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_plus, jo_token_kind_none, false);			
		break;

	case '-':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_minus, jo_token_kind_none, false);			
		break;
		
	case '*':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_star, jo_token_kind_none, false);			
		break;

	case '&':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_ampersand, jo_token_kind_none, false);			
		break;

	case '/':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_slash, jo_token_kind_none, false);			
		break;
	case ';':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_semicolon, jo_token_kind_none, false);			
		break;
	
	case ':':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_colon, jo_token_kind_none, false);			
		break;		
	case '(':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_open_paren, jo_token_kind_none, false);			
		break;

	case ')':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_close_paren, jo_token_kind_none, false);			
		break;

	case '{':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_open_curly_bracket, jo_token_kind_none, false);			
		break;

	case '}':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_close_curly_bracket, jo_token_kind_none, false);			
		break;

	case '[':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_open_square_bracket, jo_token_kind_none, false);			
		break;

	case ']':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_close_square_bracket, jo_token_kind_none, false);			
		break;
	case '<':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_open_angle_bracket, jo_token_kind_none, false);			
		break;

	case '>':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_close_angle_bracket, jo_token_kind_none, false);			
		break;

	case '=':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_equals, jo_token_kind_none, false);			
		break;
	
	case '^':
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_arrow, jo_token_kind_none, false);			
		break;

	case EOF:
		jo_lexer_handle_content_break(lexer);
		jo_lexer_make_token_maybe_consume(lexer,  jo_token_eof, jo_token_kind_none, false);			
		lexer->done = true;
		break;		
	default:
		jo_lexer_push_content(lexer);
		break;
	}
}

void jo_lexer_open(jo_lexer_t* lexer, const char* filename)
{
	lexer->stream = fopen(filename, "r");
}

void jo_lexer_close(jo_lexer_t* lexer)
{
	fclose(lexer->stream);
}

void jo_lexer_lex(jo_lexer_t* lexer)
{
	jo_lexer_newline(lexer);

	while(!lexer->done)
	{				
		jo_lexer_advance(lexer);
		jo_lexer_parse_next(lexer);
	}
}

jo_token_dyn_array_t jo_lex_file(const char* filename)
{
	jo_lexer_t lexer={0};
	
	jo_lexer_open(&lexer, filename);

	jo_lexer_lex(&lexer);

	jo_lexer_close(&lexer);

	return lexer.tokens;
}