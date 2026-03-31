#ifndef jota_bytecode
#define jota_bytecode

#include "../core/core.h"
#include "compiler_fwd.h"
#include "ast_node.h"
#include "symbol.h"

#define jo_bc_inst_cast_list(to)\
	XY(u8, to)\
	XY(u16, to)\
	XY(u32, to)\
	XY(u64, to)\
	XY(i8, to)\
	XY(i16, to)\
	XY(i32, to)\
	XY(i64, to)\
	XY(f32, to)\
	XY(f64, to)\
	XY(bool, to)

#define jo_bc_basic_instr_list\
	X(push)\
	X(pop)\
	X(jmp)\
	X(jmp_if)\
	X(jmp_if_not)\
	X(store)\
	X(load)\
	X(get_sp)\
	X(memcpy)\
	X(mov)\
	X(mov_imm)\
	X(ret)\
	X(call)

#define jo_bc_binary_primitive_instr_list(t)\
	XYX(add, t, +)\
	XYX(sub, t, -)\
	XYX(div, t, /)\
	XYX(mul, t, *)\
	XYX(cmp_lt, t, <)\
	XYX(cmp_lte, t, <=)\
	XYX(cmp_gt, t, >)\
	XYX(cmp_gte, t, >=)\
	XYX(cmp_eq, t, ==)\
	XYX(cmp_neq, t, !=)

typedef enum
{
	#define X(opcode) jo_bc_##opcode,
		jo_bc_basic_instr_list
	#undef X

	#define XYX(opcode, t, op) jo_bc_##opcode##_##t,
	#define X(t) jo_bc_binary_primitive_instr_list(t)
		jo_tok_numerical_type_primitive_list
	#undef X
	#undef XYX

	#define XY(t1, t2) jo_bc_cast_##t1##_##t2,
	#define X(t) jo_bc_inst_cast_list(t)
		jo_tok_numerical_type_primitive_list
	#undef X
	#undef XY
	
} jo_bc_instr;

const char* jo_bytecode_instr_to_str(jo_bc_instr instr);

typedef jo_u32 jo_register_id;

#define jo_null_register (jo_register_id)(1111111111)

typedef struct
{
	union
	{
		jo_register_id reg;
		jo_u32 value;
	}value;

	enum
	{
		jo_instruciton_dest_reg,
		jo_instruciton_dest_reg_deref,
		jo_instruciton_dest_value,
		jo_instruciton_dest_value_deref,
	}type;

} jo_instr_destination;

typedef jo_u64 jo_value64;

typedef struct
{
	jo_bc_instr instr;

	union
	{
		struct 
		{
			jo_register_id to;
		} get_sp;

		struct
		{
			jo_u32 offset;
		} push;
		
		struct
		{
			jo_u32 offset;
		} pop;

		struct 
		{
			jo_register_id to;
			jo_register_id from_addr;
			jo_u32 size;
		} load;

		struct
		{	
			jo_register_id to_addr;
			jo_register_id from;
			jo_u32 offset;
			jo_u32 size;
		} store;

		struct
		{
			jo_register_id target;
			jo_register_id dest;
		} cast;

		struct
		{
            jo_register_id start_reg;
    		jo_u32 size;
		} clear;

		struct
		{
            jo_register_id to_addr;
    		jo_register_id from_addr;
    		jo_u32 size;
		} memcpy;

		struct
		{
            jo_register_id to;
			jo_value64 value;
			jo_u32 size;
		} mov_imm;

		struct
		{
            jo_register_id from;
            jo_register_id to;
			jo_u32 size;
		} mov;

        struct
		{
            jo_register_id dest;
            jo_register_id a;
            jo_register_id b;
        } binary_op;

        struct
		{
            jo_u32 offset;
        } jmp;

        struct
		{
			jo_register_id cond_reg;
            jo_u32 offset;
        } jmp_if;

		struct
		{
			jo_register_id cond_reg;
            jo_u32 offset;
        } jmp_if_not;

		struct
		{
			jo_register_id first_arg;
			jo_register_id dest;
			jo_u32 arg_count;
			jo_u32 function_index;
			bool intrinsic;
			bool is_void_call;
		}call;

		struct
		{
			jo_register_id reg;
			bool is_void;
		}ret;


    } as;
}jo_bytecode_op;

jo_ada_declare(jo_bytecode_op, jo_bytecode);

typedef struct
{
	jo_str_view label;
	jo_u32 entry_ip;
	jo_register_id reg_counter;
}jo_bytecode_fn;

jo_ada_declare(jo_bytecode_fn, jo_bytecode_fn_dyn_array)

typedef struct
{
	jo_workspace* ws;
	jo_bytecode_fn_dyn_array fns;
	jo_bytecode bc;
}jo_bytecode_context;

void jo_dump_bytecode(jo_bytecode_context* bcc);
void jo_bytecode_emit_function(jo_bytecode_context* bcc,  jo_bytecode_fn* bcfn,  jo_ast_node* node);
jo_register_id jo_bytecode_emit_expr(jo_bytecode_context* bcc, jo_bytecode_fn* fn,  jo_ast_node* expr);
void jo_bytecode_emit_block(jo_bytecode_context* bcc, jo_bytecode_fn* fn, jo_ast_block* ast_block);
void jo_bytecode_dump_op(jo_bytecode_context* bcc, jo_bytecode_op* op);
jo_u32 jo_bytecode_find_function_id(jo_bytecode_context* bcc, const char* identifier);
void jo_make_bytecode(jo_bytecode_context* bcc);

#endif
