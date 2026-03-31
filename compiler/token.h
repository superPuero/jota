#ifndef jota_token
#define jota_token

#include "../core/core.h"

#define jo_tok_literal_list\
	X(literal_integer)\
	X(literal_fp)\
	X(literal_string)

#define jo_tok_operator_list\
	X(plus)\
	X(minus)\
	X(star)\
	X(slash)\
	X(modulo)\
	X(plus_equals)\
	X(minus_equals)\
	X(star_equals)\
	X(slash_equals)\
	X(modulo_equals)\
	X(bitwise_or)\
	X(bitwise_xor)\
	X(logical_and)\
	X(logical_or)\
	X(logical_xor)\
	X(bitwise_and_equals)\
	X(bitwise_or_equals)\
	X(bitwise_xor_equals)\
	X(less_equals)\
	X(greater_equals)\
	X(not_equals)\
	X(shift_left)\
	X(shift_right)\
	X(shift_left_equals)\
	X(shift_right_equals)\
	X(double_equals)



#define jo_tok_unsigned_integer_type_primitive_list\
	X(u8)\
	X(u16)\
	X(u32)\
	X(u64)

#define jo_tok_signed_integer_type_primitive_list\
	X(i8)\
	X(i16)\
	X(i32)\
	X(i64)

#define jo_tok_floating_point_type_primitive_list\
	X(f32)\
	X(f64)


#define jo_tok_numerical_type_primitive_list\
	X(bool)\
	jo_tok_unsigned_integer_type_primitive_list\
	jo_tok_signed_integer_type_primitive_list\
	jo_tok_floating_point_type_primitive_list

	
#define jo_tok_type_primitive_list\
	X(type)\
	X(void)\
	jo_tok_numerical_type_primitive_list

#define jo_tok_keyword_list\
	X(true)\
	X(false)\
	X(as)\
	X(return)\
	X(struct)\
	X(fn)\
	X(let)\
	X(if)\
	X(else)\
	X(match)\
	X(nil)\
	X(enum)\
	X(static)\
	X(import)\
	X(module)\
	X(defer)\
	X(for)\
	X(in)\
	X(break)\
	X(continue)\
	X(namespace)\
	jo_tok_type_primitive_list

#define jo_tok_entry_list\
	X(undefined)\
	X(identifier)\
	X(intrinsic)\
	X(open_parenthesis)\
	X(close_parenthesis)\
	X(open_curly_bracket)\
	X(close_curly_bracket)\
	X(open_angle_bracket)\
	X(close_angle_bracket)\
	X(open_square_bracket)\
	X(close_square_bracket)\
	X(exclamation_mark)\
	X(hash)\
	X(at)\
	X(dollar)\
	X(comma)\
	X(dot)\
	X(pipe)\
	X(tilde)\
	X(arrow)\
	X(ampersand)\
	X(equals)\
	X(double_dot)\
	X(fat_arrow)\
	X(bridge)\
	X(walrus)\
	X(colon)\
	X(semicolon)\
	X(eof)\
	jo_tok_operator_list\
	jo_tok_keyword_list\
	jo_tok_literal_list

typedef enum
{
	#define X(tok) jo_tok_##tok,
	jo_tok_entry_list
	#undef X
	
	jo_tok_add = jo_tok_plus,
	jo_tok_sub = jo_tok_minus,
	jo_tok_mul = jo_tok_star,
	jo_tok_div = jo_tok_slash,

	jo_tok_cmp_lt = jo_tok_open_angle_bracket,
	jo_tok_cmp_lte = jo_tok_less_equals,
	jo_tok_cmp_gt = jo_tok_close_angle_bracket,
	jo_tok_cmp_gte = jo_tok_greater_equals,
	jo_tok_cmp_eq = jo_tok_double_equals,
	jo_tok_cmp_neq = jo_tok_not_equals,
	jo_tok_caret = jo_tok_logical_xor,
	jo_tok_bitwise_and = jo_tok_ampersand,	
} jo_tok;

typedef struct
{	
	const char* identifier;
	jo_u32 len;
	jo_tok type;
} jo_tok_keyword_entry;	

typedef struct
{
	const char* content;
	jo_u32 len;
	jo_tok type;
} jo_token_basic_entry;
 
const char* jo_tok_to_string(jo_tok token);

jo_u32 jo_tok_binary_operator_precedence(jo_tok token);

typedef struct
{
	jo_tok type;
	char* content;
	jo_u32 content_len;	
	jo_u32 line; 
	jo_u32 column; 	
} jo_token;


jo_uz jo_tok_get_type_primitive_size(jo_tok tok);
jo_bool jo_tok_is_unsigned_integer_type_primitive(jo_tok tok);
jo_bool jo_tok_is_signed_integer_type_primitive(jo_tok tok);
jo_bool jo_tok_is_integer_type_primitive(jo_tok tok);
jo_bool jo_tok_is_fp_type_primitive(jo_tok tok);
jo_bool jo_tok_is_numerical(jo_tok tok);
jo_bool jo_tok_is_literal(jo_tok tok);
jo_bool jo_tok_is_type_primitive(jo_tok tok);
jo_bool jo_tok_is_operator(jo_tok tok);
jo_tok jo_tok_pick_primitive_upcast(jo_tok l, jo_tok r);

jo_ada_declare(jo_token, jo_token_ada);

#endif