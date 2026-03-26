#ifndef jota_token
#define jota_token

#include "../core/core.h"

#define jo_token_content_max_length 64

#define jo_token_make_keyword_enum(keyword)\
	jo_token_keyword_##keyword

#define jo_token_make_keyword_entry(keyword, kind)\
	{#keyword, jo_token_keyword_##keyword, kind}

typedef enum
{
	jo_token_undefined = 0,
	jo_token_identifier,
	jo_token_literal_u8,
	jo_token_literal_i8,
	jo_token_literal_u16,
	jo_token_literal_i16,
	jo_token_literal_u32,
	jo_token_literal_i32,	
	jo_token_literal_u64,
	jo_token_literal_i64,
	jo_token_literal_f32,
	jo_token_literal_f64,
	jo_token_literal_string,

	jo_token_make_keyword_enum(type),	
	jo_token_make_keyword_enum(i8),
	jo_token_make_keyword_enum(u8),
	jo_token_make_keyword_enum(i16),
	jo_token_make_keyword_enum(u16),
	jo_token_make_keyword_enum(i32),
	jo_token_make_keyword_enum(u32),
	jo_token_make_keyword_enum(i64),
	jo_token_make_keyword_enum(u64),
	jo_token_make_keyword_enum(f32),
	jo_token_make_keyword_enum(f64),
	jo_token_make_keyword_enum(bool),
	jo_token_make_keyword_enum(void),
	jo_token_make_keyword_enum(true),
	jo_token_make_keyword_enum(false),
	jo_token_make_keyword_enum(as),
	jo_token_make_keyword_enum(return),
	jo_token_make_keyword_enum(struct),
	jo_token_make_keyword_enum(fn),
	jo_token_make_keyword_enum(let),
	jo_token_make_keyword_enum(if),
	jo_token_make_keyword_enum(else),
	jo_token_make_keyword_enum(match),
	jo_token_make_keyword_enum(nil),
	jo_token_make_keyword_enum(enum),
	jo_token_make_keyword_enum(static),
	jo_token_make_keyword_enum(import),
	jo_token_make_keyword_enum(module),
	jo_token_make_keyword_enum(defer),
	jo_token_make_keyword_enum(for),
	jo_token_make_keyword_enum(in),
	jo_token_make_keyword_enum(break),
	jo_token_make_keyword_enum(continue),
	jo_token_make_keyword_enum(namespace),
	jo_token_make_keyword_enum(load),
	jo_token_make_keyword_enum(intrinsic),

	jo_token_open_parenthesis,
	jo_token_close_parenthesis,
	jo_token_open_curly_bracket,
	jo_token_close_curly_bracket,
	jo_token_open_angle_bracket,
	jo_token_close_angle_bracket,
	jo_token_open_square_bracket,
	jo_token_close_square_bracket,
	jo_token_exclamation_mark,
	jo_token_hash,
	jo_token_at,
	jo_token_dollar,
	jo_token_comma,
	jo_token_dot,

	jo_token_plus,
	jo_token_minus,
	jo_token_star,
	jo_token_slash,
	jo_token_modulo,
	
	jo_token_plus_equals,
	jo_token_minus_equals,
	jo_token_star_equals,
	jo_token_slash_equals,
	jo_token_modulo_equals,

	
	jo_token_pipe,
	jo_token_tilde,

	
	jo_token_bitwise_or,
	jo_token_bitwise_xor,
	
	jo_token_logical_and,
	jo_token_logical_or,
	jo_token_logical_xor,
	
	jo_token_bitwise_and_equals,
	jo_token_bitwise_or_equals,
	jo_token_bitwise_xor_equals,	

	jo_token_less_equals,
	jo_token_greater_equals,

	jo_token_arrow,

	jo_token_ampersand,
	jo_token_equals,
	jo_token_double_equals,
	jo_token_not_equals,
	jo_token_shift_left,
	jo_token_shift_right,
	jo_token_shift_left_equals,
	jo_token_shift_right_equals,
	jo_token_fat_arrow,
	jo_token_bridge,
	jo_token_walrus,
	jo_token_colon,
	jo_token_semicolon,
	jo_token_eof,
	jo_token_less = jo_token_open_angle_bracket,
	jo_token_greater = jo_token_close_angle_bracket,
	jo_token_caret = jo_token_logical_xor,
	jo_token_bitwise_and = jo_token_ampersand,
} jo_token_type_t;

typedef enum 
{
	jo_token_kind_none = 0,
	jo_token_kind_type_primitive,
	jo_token_kind_type_operator,
	jo_token_kind_literal
} jo_token_kind;

typedef struct
{
	const char* identifier;
	jo_token_type_t type;
	jo_token_kind kind;
} jo_token_keyword_entry;
 
const char* jo_token_type_to_string(jo_token_type_t token);

jo_u32 jo_token_binary_operator_precedence(jo_token_type_t token);

typedef struct
{
	jo_token_type_t type;
	char* content;
	jo_u32 content_len;	
	jo_u32 line; 
	jo_u32 column; 	
	jo_token_kind kind;
} jo_token_t;

// jo_decl_dyn_array_named(jo_token_t, jo_token_dyn_array_t);

#endif