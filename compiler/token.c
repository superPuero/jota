#include "token.h"
#include <assert.h>

#define stringify(x) #x

const char* tok_to_string(tok token)
{
	switch (token)
	{
		#define X(tok) stringify_case(tok_##tok);
		tok_entry_list
		#undef X
		default:
			return "invalid_token";
	}
}

bool8 tok_is_unsigned_integer_type_primitive(tok tok)
{
	switch(tok)
	{
		#define X(t) case tok_##t: return true;
		tok_unsigned_integer_type_primitive_list
		#undef X

		default:
			return false;
	}
}
bool8 tok_is_signed_integer_type_primitive(tok tok)
{
	switch(tok)
	{
		#define X(t) case tok_##t: return true;
		tok_signed_integer_type_primitive_list
		#undef X

		default:
			return false;
	}
}
bool8 tok_is_integer_type_primitive(tok tok)
{
	return tok_is_signed_integer_type_primitive(tok) || tok_is_unsigned_integer_type_primitive(tok);
}

bool8 tok_is_fp_type_primitive(tok tok)
{
	switch(tok)
	{
		#define X(t) case tok_##t: return true;
		tok_floating_point_type_primitive_list
		#undef X

		default:
			return false;
	}
}

//@thought: this is kinda shady
uz tok_get_type_primitive_size(tok tok)
{
	switch(tok)
	{
		#define X(t) case tok_##t: return sizeof(t);
			tok_numerical_type_primitive_list
		#undef X
			case tok_void: return 0;
		default:
			assert(0);
			return -1;
	}
}

u32 tok_get_type_weight(tok tok)
{
    switch(tok)
    {
        case tok_bool: return 1;

        case tok_i8:   return 10;
        case tok_u8:   return 11;
        case tok_i16:  return 20;
        case tok_u16:  return 21;
        case tok_i32:  return 30;
        case tok_u32:  return 31;
        case tok_i64:  return 40;
        case tok_u64:  return 41;

        case tok_f32:  return 100;
        case tok_f64:  return 110;

        default: return 0;
    }
}
tok tok_pick_primitive_upcast(tok l, tok r)
{
	return tok_get_type_weight(l) >= tok_get_type_weight(r) ? l : r;
}


// tok tok_pick_primitive_upcast(tok l, tok r)
// {
// 	if(tok_is_fp_type_primitive(l))
// 	{
// 		if(tok_is_fp_type_primitive(r)) { return tok_get_type_primitive_size(l) > tok_get_type_primitive_size(r) ? l : r; }
// 		else { return l; }
// 	}
// 	else
// 	{
// 		if(tok_is_fp_type_primitive(r)) { return r; }
// 		else { return tok_get_type_primitive_size(l) > tok_get_type_primitive_size(r) ? l : r; }
// 	}
//}

bool8 tok_is_numerical(tok tok)
{
	switch(tok)
	{
		#define X(t) case tok_##t: return true;
		tok_numerical_type_primitive_list
		#undef X

		default:
			return false;
	}
}

bool8 tok_is_literal(tok tok)
{
	switch(tok)
	{
		#define X(t) case tok_##t: return true;
		tok_literal_list
		#undef X

		default:
			return false;
	}
}

bool8 tok_is_type_primitive(tok tok)
{
	switch(tok)
	{
		#define X(t) case tok_##t: return true;
		tok_type_primitive_list
		#undef X

		default:
			return false;
	}
}

bool8 tok_is_operator(tok tok)
{
	switch(tok)
	{
		#define X(t) case tok_##t: return true;
		tok_operator_list
		#undef X
		
		default:
			return false;
	}
}

u32 tok_binary_operator_precedence(tok token)
{
	switch (token)
	{
	case tok_plus_equals:             // +
	case tok_minus_equals:			 // -
		return 4;

 	case tok_equals:       			 // =
		return 5;

    case tok_logical_or:        // ||
        return 10;
		
		case tok_logical_and:       // &&
        return 11;

    case tok_double_equals:         // ==
    case tok_not_equals:     // !=
        return 15;

    case tok_cmp_lt:              // <
    case tok_cmp_gt:           // >
    case tok_cmp_lte:        // <=
    case tok_cmp_gte:     // >=
		return 20;

    case tok_modulo:
    case tok_modulo_equals:              // +
        return 22;

    case tok_plus:
    case tok_minus:
		return 25;

    case tok_star:        // *
    case tok_star_equals:        // *
    case tok_slash:     // /
    case tok_slash_equals:     // /
		return 30;

	default:
		return 0;
	}
}
