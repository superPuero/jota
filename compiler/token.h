#ifndef jota_token
#define jota_token

#include "../core/core.h"

#define jo_token_content_max_length 64

typedef enum
{
	jo_token_undefined = 0,

	jo_token_identifier,

	jo_token_literal_integer,
	jo_token_literal_string,
	
	jo_token_keyword_i32,
	jo_token_keyword_u32,
	jo_token_keyword_bool,
	jo_token_keyword_void,

	jo_token_keyword_true,
	jo_token_keyword_false,
	jo_token_keyword_return,
	jo_token_keyword_struct,
	jo_token_keyword_fn,
	jo_token_keyword_let,
	jo_token_keyword_if,
	jo_token_keyword_else,
	jo_token_keyword_match,
	jo_token_keyword_nil,
	jo_token_keyword_enum,
	jo_token_keyword_static,
	jo_token_keyword_defer,
	
	jo_token_hash,
	jo_token_comma,
	jo_token_plus,
	jo_token_minus,
	jo_token_star,
	jo_token_slash,
	jo_token_arrow,
	jo_token_ampersand,
	jo_token_colon,
	jo_token_equals,
	jo_token_semicolon,
	jo_token_open_paren,
	jo_token_close_paren,
	jo_token_open_curly_bracket,
	jo_token_close_curly_bracket,
	jo_token_open_angle_bracket,
	jo_token_close_angle_bracket,
	jo_token_open_square_bracket,
	jo_token_close_square_bracket,

	jo_token_eof

} jo_token_type_t;

const char* jo_token_type_to_string(jo_token_type_t token);
jo_u32 jo_token_binary_operator_precedence(jo_token_type_t token);


typedef enum 
{
	jo_token_kind_none = 0,
	jo_token_kind_type_primitive,
} jo_token_kind;

typedef struct
{
	jo_token_type_t type;
	char content[jo_token_content_max_length];
	jo_u32 line; 
	jo_u32 column; 
	jo_token_kind kind;
} jo_token_t;

jo_decl_dyn_array_named(jo_token_t, jo_token_dyn_array_t);

#endif