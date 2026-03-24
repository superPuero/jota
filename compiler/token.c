#include "token.h"


const char* jo_token_type_to_string(jo_token_type_t token)
{
	switch (token)
	{
	jo_stringify_case(jo_token_undefined);
	jo_stringify_case(jo_token_identifier);
	jo_stringify_case(jo_token_literal_u8);
	jo_stringify_case(jo_token_literal_i8);
	jo_stringify_case(jo_token_literal_u16);
	jo_stringify_case(jo_token_literal_i16);
	jo_stringify_case(jo_token_literal_u32);
	jo_stringify_case(jo_token_literal_i32);
	jo_stringify_case(jo_token_literal_u64);
	jo_stringify_case(jo_token_literal_i64);
	jo_stringify_case(jo_token_literal_f32);
	jo_stringify_case(jo_token_literal_f64);
	jo_stringify_case(jo_token_literal_string);
	jo_stringify_case(jo_token_keyword_type);
	jo_stringify_case(jo_token_keyword_i8);
	jo_stringify_case(jo_token_keyword_u8);
	jo_stringify_case(jo_token_keyword_i16);
	jo_stringify_case(jo_token_keyword_u16);
	jo_stringify_case(jo_token_keyword_i32);
	jo_stringify_case(jo_token_keyword_u32);
	jo_stringify_case(jo_token_keyword_i64);
	jo_stringify_case(jo_token_keyword_u64);
	jo_stringify_case(jo_token_keyword_f32);
	jo_stringify_case(jo_token_keyword_f64);
	jo_stringify_case(jo_token_keyword_bool);
	jo_stringify_case(jo_token_keyword_void);
	jo_stringify_case(jo_token_keyword_true);
	jo_stringify_case(jo_token_keyword_false);
	jo_stringify_case(jo_token_keyword_as);
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
	jo_stringify_case(jo_token_keyword_import);
	jo_stringify_case(jo_token_keyword_defer);
	jo_stringify_case(jo_token_keyword_for);
	jo_stringify_case(jo_token_keyword_in);
	jo_stringify_case(jo_token_keyword_break);
	jo_stringify_case(jo_token_keyword_continue);
	jo_stringify_case(jo_token_keyword_namespace);
	jo_stringify_case(jo_token_keyword_load);
	jo_stringify_case(jo_token_keyword_intrinsic);
	jo_stringify_case(jo_token_open_parenthesis);
	jo_stringify_case(jo_token_close_parenthesis);
	jo_stringify_case(jo_token_open_curly_bracket);
	jo_stringify_case(jo_token_close_curly_bracket);
	jo_stringify_case(jo_token_open_angle_bracket);
	jo_stringify_case(jo_token_close_angle_bracket);
	jo_stringify_case(jo_token_open_square_bracket);
	jo_stringify_case(jo_token_close_square_bracket);
	jo_stringify_case(jo_token_exclamation_mark);
	jo_stringify_case(jo_token_hash);
	jo_stringify_case(jo_token_at);
	jo_stringify_case(jo_token_dollar);
	jo_stringify_case(jo_token_comma);
	jo_stringify_case(jo_token_dot);
	jo_stringify_case(jo_token_plus);
	jo_stringify_case(jo_token_minus);
	jo_stringify_case(jo_token_star);
	jo_stringify_case(jo_token_slash);

	jo_stringify_case(jo_token_modulo);
	jo_stringify_case(jo_token_plus_equals);
	jo_stringify_case(jo_token_minus_equals);
	jo_stringify_case(jo_token_star_equals);
	jo_stringify_case(jo_token_slash_equals);
	jo_stringify_case(jo_token_modulo_equals);

	jo_stringify_case(jo_token_less_equals);
	jo_stringify_case(jo_token_greater_equals);

	jo_stringify_case(jo_token_arrow);
	jo_stringify_case(jo_token_caret);
	jo_stringify_case(jo_token_ampersand);
	jo_stringify_case(jo_token_equals);
	jo_stringify_case(jo_token_double_equals);
	jo_stringify_case(jo_token_not_equals);
	jo_stringify_case(jo_token_shift_left);
	jo_stringify_case(jo_token_shift_right);
	jo_stringify_case(jo_token_shift_left_equals);
	jo_stringify_case(jo_token_shift_right_equals);
	jo_stringify_case(jo_token_fat_arrow);
	jo_stringify_case(jo_token_bridge);
	jo_stringify_case(jo_token_walrus);
	jo_stringify_case(jo_token_colon);
	jo_stringify_case(jo_token_semicolon);
	jo_stringify_case(jo_token_eof);
	default:
		return "invalid_token";
	}
}

jo_u32 jo_token_binary_operator_precedence(jo_token_type_t token)
{
	switch (token)
	{
 	case jo_token_equals:        // =
		return 5;

    case jo_token_logical_or:        // ||
    case jo_token_logical_and:       // &&
        return 10;

    case jo_token_double_equals:         // ==
    case jo_token_not_equals:     // !=
        return 15;

    case jo_token_less:              // <
    case jo_token_greater:           // >
    case jo_token_less_equals:        // <=
    case jo_token_greater_equals:     // >=
		return 20;

    case jo_token_modulo:
    case jo_token_modulo_equals:              // +
        return 22;

    case jo_token_plus:
    case jo_token_plus_equals:              // +
    case jo_token_minus:
    case jo_token_minus_equals:			// -
		return 25;
    case jo_token_star:        // *
    case jo_token_star_equals:        // *
    case jo_token_slash:     // /
    case jo_token_slash_equals:     // /
		return 30;

	default:
		return 0;
	}
}
