#include "token.h"
#include <assert.h>

#define jo_stringify(x) #x

const char* jo_tok_to_string(jo_tok token)
{
	switch (token)
	{
		#define X(tok) jo_stringify_case(jo_tok_##tok);
		jo_tok_entry_list
		#undef X
		default:
			return "invalid_token";
	}
}

jo_bool jo_tok_is_unsigned_integer_type_primitive(jo_tok tok)
{
	switch(tok)
	{
		#define X(t) case jo_tok_##t: return true;
		jo_tok_unsigned_integer_type_primitive_list
		#undef X

		default:
			return false;
	}
}
jo_bool jo_tok_is_signed_integer_type_primitive(jo_tok tok)
{
	switch(tok)
	{
		#define X(t) case jo_tok_##t: return true;
		jo_tok_signed_integer_type_primitive_list
		#undef X

		default:
			return false;
	}
}
jo_bool jo_tok_is_integer_type_primitive(jo_tok tok)
{
	return jo_tok_is_signed_integer_type_primitive(tok) || jo_tok_is_unsigned_integer_type_primitive(tok);
}

jo_bool jo_tok_is_fp_type_primitive(jo_tok tok)
{
	switch(tok)
	{
		#define X(t) case jo_tok_##t: return true;
		jo_tok_floating_point_type_primitive_list
		#undef X

		default:
			return false;
	}
}

//@thought: this is kinda shady
jo_uz jo_tok_get_type_primitive_size(jo_tok tok)
{
	switch(tok)
	{
		#define X(t) case jo_tok_##t: return sizeof(jo_##t);
			jo_tok_numerical_type_primitive_list
		#undef X
			case jo_tok_void: return 0;
		default:
			assert(0);
			return -1;
	}
}

jo_u32 jo_tok_get_type_weight(jo_tok tok)
{
    switch(tok)
    {
        case jo_tok_bool: return 1;

        case jo_tok_i8:   return 10;
        case jo_tok_u8:   return 11;
        case jo_tok_i16:  return 20;
        case jo_tok_u16:  return 21;
        case jo_tok_i32:  return 30;
        case jo_tok_u32:  return 31;
        case jo_tok_i64:  return 40;
        case jo_tok_u64:  return 41;

        case jo_tok_f32:  return 100;
        case jo_tok_f64:  return 110;

        default: return 0;
    }
}
jo_tok jo_tok_pick_primitive_upcast(jo_tok l, jo_tok r)
{
	return jo_tok_get_type_weight(l) >= jo_tok_get_type_weight(r) ? l : r;
}


// jo_tok jo_tok_pick_primitive_upcast(jo_tok l, jo_tok r)
// {
// 	if(jo_tok_is_fp_type_primitive(l))
// 	{
// 		if(jo_tok_is_fp_type_primitive(r)) { return jo_tok_get_type_primitive_size(l) > jo_tok_get_type_primitive_size(r) ? l : r; }
// 		else { return l; }
// 	}
// 	else
// 	{
// 		if(jo_tok_is_fp_type_primitive(r)) { return r; }
// 		else { return jo_tok_get_type_primitive_size(l) > jo_tok_get_type_primitive_size(r) ? l : r; }
// 	}
//}

jo_bool jo_tok_is_numerical(jo_tok tok)
{
	switch(tok)
	{
		#define X(t) case jo_tok_##t: return true;
		jo_tok_numerical_type_primitive_list
		#undef X

		default:
			return false;
	}
}

jo_bool jo_tok_is_literal(jo_tok tok)
{
	switch(tok)
	{
		#define X(t) case jo_tok_##t: return true;
		jo_tok_literal_list
		#undef X

		default:
			return false;
	}
}

jo_bool jo_tok_is_type_primitive(jo_tok tok)
{
	switch(tok)
	{
		#define X(t) case jo_tok_##t: return true;
		jo_tok_type_primitive_list
		#undef X

		default:
			return false;
	}
}

jo_bool jo_tok_is_operator(jo_tok tok)
{
	switch(tok)
	{
		#define X(t) case jo_tok_##t: return true;
		jo_tok_operator_list
		#undef X
		
		default:
			return false;
	}
}

jo_u32 jo_tok_binary_operator_precedence(jo_tok token)
{
	switch (token)
	{
	case jo_tok_plus_equals:             // +
	case jo_tok_minus_equals:			 // -
		return 4;

 	case jo_tok_equals:       			 // =
		return 5;

    case jo_tok_logical_or:        // ||
        return 10;
		
		case jo_tok_logical_and:       // &&
        return 11;

    case jo_tok_double_equals:         // ==
    case jo_tok_not_equals:     // !=
        return 15;

    case jo_tok_cmp_lt:              // <
    case jo_tok_cmp_gt:           // >
    case jo_tok_cmp_lte:        // <=
    case jo_tok_cmp_gte:     // >=
		return 20;

    case jo_tok_modulo:
    case jo_tok_modulo_equals:              // +
        return 22;

    case jo_tok_plus:
    case jo_tok_minus:
		return 25;

    case jo_tok_star:        // *
    case jo_tok_star_equals:        // *
    case jo_tok_slash:     // /
    case jo_tok_slash_equals:     // /
		return 30;

	default:
		return 0;
	}
}
