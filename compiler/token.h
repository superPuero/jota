#ifndef jota_token
#define jota_token

#include "../core/core.h"

#define tok_literal_list\
	X(literal_integer)\
	X(literal_fp)\
	X(literal_string)

#define tok_operator_list\
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



#define tok_unsigned_integer_type_primitive_list\
	X(u8)\
	X(u16)\
	X(u32)\
	X(u64)

#define tok_signed_integer_type_primitive_list\
	X(i8)\
	X(i16)\
	X(i32)\
	X(i64)

#define tok_floating_point_type_primitive_list\
	X(f32)\
	X(f64)


#define tok_numerical_type_primitive_list\
	X(bool)\
	tok_unsigned_integer_type_primitive_list\
	tok_signed_integer_type_primitive_list\
	tok_floating_point_type_primitive_list

	
#define tok_type_primitive_list\
	X(type)\
	X(void)\
	tok_numerical_type_primitive_list

#define tok_keyword_list\
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
	tok_type_primitive_list

#define tok_entry_list\
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
	tok_operator_list\
	tok_keyword_list\
	tok_literal_list

typedef enum
{
	#define X(tok) tok_##tok,
	tok_entry_list
	#undef X
	
	tok_add = tok_plus,
	tok_sub = tok_minus,
	tok_mul = tok_star,
	tok_div = tok_slash,

	tok_cmp_lt = tok_open_angle_bracket,
	tok_cmp_lte = tok_less_equals,
	tok_cmp_gt = tok_close_angle_bracket,
	tok_cmp_gte = tok_greater_equals,
	tok_cmp_eq = tok_double_equals,
	tok_cmp_neq = tok_not_equals,
	tok_caret = tok_logical_xor,
	tok_bitwise_and = tok_ampersand,	
} tok;

typedef struct
{	
	const char* identifier;
	u32 len;
	tok type;
} tok_keyword_entry;	

typedef struct
{
	const char* content;
	u32 len;
	tok type;
} token_basic_entry;
 
const char* tok_to_string(tok token);

u32 tok_binary_operator_precedence(tok token);

typedef struct
{
	tok type;
	char* content;
	u32 content_len;	
	u32 line; 
	u32 column; 	
} token;


uz tok_get_type_primitive_size(tok tok);
bool8 tok_is_unsigned_integer_type_primitive(tok tok);
bool8 tok_is_signed_integer_type_primitive(tok tok);
bool8 tok_is_integer_type_primitive(tok tok);
bool8 tok_is_fp_type_primitive(tok tok);
bool8 tok_is_numerical(tok tok);
bool8 tok_is_literal(tok tok);
bool8 tok_is_type_primitive(tok tok);
bool8 tok_is_operator(tok tok);
tok tok_pick_primitive_upcast(tok l, tok r);

ada_declare(token, token_ada);

#endif