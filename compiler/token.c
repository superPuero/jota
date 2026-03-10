#include "token.h"


const char* jo_token_type_to_string(jo_token_type_t token)
{
	switch (token)
	{
	jo_stringify_case(jo_token_undefined);	
	jo_stringify_case(jo_token_keyword_i32);
	jo_stringify_case(jo_token_keyword_u32);
	jo_stringify_case(jo_token_keyword_bool);
	jo_stringify_case(jo_token_keyword_void);
	jo_stringify_case(jo_token_keyword_true);
	jo_stringify_case(jo_token_keyword_false);
	jo_stringify_case(jo_token_keyword_return);
	jo_stringify_case(jo_token_keyword_struct);
	jo_stringify_case(jo_token_keyword_fn);
	jo_stringify_case(jo_token_keyword_let);
	jo_stringify_case(jo_token_keyword_if);
	jo_stringify_case(jo_token_keyword_else);
	jo_stringify_case(jo_token_keyword_match);
	jo_stringify_case(jo_token_keyword_nil);
	jo_stringify_case(jo_token_keyword_enum);
	jo_stringify_case(jo_token_keyword_static);
	jo_stringify_case(jo_token_keyword_defer);

	jo_stringify_case(jo_token_identifier);
	jo_stringify_case(jo_token_comma);
	jo_stringify_case(jo_token_plus);
	jo_stringify_case(jo_token_minus);
	jo_stringify_case(jo_token_star);
	jo_stringify_case(jo_token_slash);
	jo_stringify_case(jo_token_arrow);
	jo_stringify_case(jo_token_open_angle_bracket);
	jo_stringify_case(jo_token_close_angle_bracket);


	jo_stringify_case(jo_token_ampersand);
	jo_stringify_case(jo_token_colon);
	jo_stringify_case(jo_token_equals);
	jo_stringify_case(jo_token_semicolon);
	jo_stringify_case(jo_token_open_paren);
	jo_stringify_case(jo_token_close_paren);
	jo_stringify_case(jo_token_open_curly_bracket);
	jo_stringify_case(jo_token_close_curly_bracket);
	jo_stringify_case(jo_token_open_square_bracket);
	jo_stringify_case(jo_token_close_square_bracket);
	jo_stringify_case(jo_token_literal_integer);
	jo_stringify_case(jo_token_eof);	

	default:
		return "invalid_token";
	}	
}

jo_u32 jo_token_binary_operator_precedence(jo_token_type_t token)
{
	switch (token)
	{
	case jo_token_minus:
	case jo_token_plus:
		return 5;
		break;
	case jo_token_star:
	case jo_token_slash:
		return 10;
		break;

		
	default:
		return 0;
		break;
	}
}
