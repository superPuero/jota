#ifndef jota_ast_node
#define jota_ast_node

#include "../core/core.h"
#include "token.h"

typedef enum 
{
	jo_ast_entry,
	jo_ast_type,
	jo_ast_type_primitive,
	jo_ast_type_function,
	jo_ast_type_struct,
	jo_ast_type_tuple,

	jo_ast_literal_integer,
	jo_ast_literal_string,
	jo_ast_literal_bool,

	jo_ast_identifier,	
	jo_ast_declaration,
	jo_ast_unary_opperation,
	jo_ast_binary_opperation,
	jo_ast_apply_operation,
	jo_ast_expression_if,

	jo_ast_statement_expression,
	jo_ast_statement_declaration,
	jo_ast_statement_return,

	jo_ast_block
} jo_ast_node_type_t;

const char* jo_ast_node_type_to_stirng(jo_ast_node_type_t type);

typedef struct jo_ast_node jo_ast_node_t;

jo_decl_dyn_array_named(jo_ast_node_t, jo_ast_node_dyn_array_t);
jo_decl_dyn_array_named(jo_ast_node_t*, jo_ast_node_ptr_dyn_array_t);

struct jo_ast_node
{	
	jo_ast_node_type_t type;
	union 
	{
		jo_token_type_t type_primitive;

        jo_i32 literal_integer; 
        bool literal_bool; 

        char literal_string[jo_token_content_max_length]; 

        char identifier[jo_token_content_max_length]; 

		struct
		{
			jo_ast_node_ptr_dyn_array_t declarations;
		} entry;

		struct
		{			
			jo_ast_node_ptr_dyn_array_t statements;
			jo_ast_node_t* yield_expression;
		} block;

		struct 
		{
			jo_ast_node_t* condition;  
			jo_ast_node_t* true_branch;
			jo_ast_node_t* false_branch;
		} expression_if;

        struct 
		{
            jo_ast_node_t* left_expression;
            jo_ast_node_t* right_expression;
            jo_token_type_t operator_type;
        } operation_binary;

		struct 
		{
            jo_ast_node_t* expression;
            jo_token_type_t operator_type;
        } operation_unary;

        struct 
		{
            jo_ast_node_t* expression;
        } statement_return;

		struct 
		{
            jo_ast_node_t* expression;			
		} statement_expression;

		
		struct 
		{
            jo_ast_node_t* declaration;			
		} statement_declaration;


		struct 
		{
			jo_ast_node_t* target;
			jo_ast_node_ptr_dyn_array_t arguments;
			jo_token_type_t  apply_type;
		} operation_apply;
	
		struct
		{
			jo_ast_node_t* identifier;						
			jo_ast_node_t* type;
			jo_ast_node_t* initialize_expression;
		} declaration;		
		
		struct 
		{
			jo_ast_node_t* inner;
			bool pointer;
			bool reference;
			bool array;
			jo_ast_node_t*  array_size_expression;
		} type;

		struct 
		{
			jo_ast_node_ptr_dyn_array_t parameters;
			jo_ast_node_t* return_type;
		} type_function;

		struct 		
		{
			jo_ast_node_ptr_dyn_array_t members;			
		} type_struct;
		

		struct 
		{
			jo_ast_node_ptr_dyn_array_t entries;
		} type_tuple;

    } data;
};

#endif