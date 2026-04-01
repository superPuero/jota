#ifndef jota_bytecode
#define jota_bytecode

#include "../core/core.h"
#include "compiler_fwd.h"
#include "ast_node.h"
#include "symbol.h"

#define bc_inst_cast_list(to)\
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

#define bc_basic_instr_list\
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

#define bc_binary_primitive_instr_list(t)\
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
	#define X(opcode) bc_##opcode,
		bc_basic_instr_list
	#undef X

	#define XYX(opcode, t, op) bc_##opcode##_##t,
	#define X(t) bc_binary_primitive_instr_list(t)
		tok_numerical_type_primitive_list
	#undef X
	#undef XYX

	#define XY(t1, t2) bc_cast_##t1##_##t2,
	#define X(t) bc_inst_cast_list(t)
		tok_numerical_type_primitive_list
	#undef X
	#undef XY
	
} bc_instr;

const char* bytecode_instr_to_str(bc_instr instr);

typedef u32 register_id;

#define null_register (register_id)(1111111111)

typedef struct
{
	union
	{
		register_id reg;
		u32 value;
	}value;

	enum
	{
		instruciton_dest_reg,
		instruciton_dest_reg_deref,
		instruciton_dest_value,
		instruciton_dest_value_deref,
	}type;

} instr_destination;

typedef u64 value64;

typedef struct
{
	bc_instr instr;

	union
	{
		struct 
		{
			register_id to;
		} get_sp;

		struct
		{
			u32 offset;
		} push;
		
		struct
		{
			u32 offset;
		} pop;

		struct 
		{
			register_id to;
			register_id from_addr;
			u32 size;
		} load;

		struct
		{	
			register_id to_addr;
			register_id from;
			u32 offset;
			u32 size;
		} store;

		struct
		{
			register_id target;
			register_id dest;
		} cast;

		struct
		{
            register_id start_reg;
    		u32 size;
		} clear;

		struct
		{
            register_id to_addr;
    		register_id from_addr;
    		u32 size;
		} memcpy;

		struct
		{
            register_id to;
			value64 value;
			u32 size;
		} mov_imm;

		struct
		{
            register_id from;
            register_id to;
			u32 size;
		} mov;

        struct
		{
            register_id dest;
            register_id a;
            register_id b;
        } binary_op;

        struct
		{
            u32 offset;
        } jmp;

        struct
		{
			register_id cond_reg;
            u32 offset;
        } jmp_if;

		struct
		{
			register_id cond_reg;
            u32 offset;
        } jmp_if_not;

		struct
		{
			register_id first_arg;
			register_id dest;
			u32 arg_count;
			u32 function_index;
			bool8 intrinsic;
			bool8 is_void_call;
		}call;

		struct
		{
			register_id reg;
			bool8 is_void;
		}ret;


    } as;
}bytecode_op;

ada_declare(bytecode_op, bytecode);

typedef struct
{
	str_view label;
	u32 entry_ip;
	register_id reg_counter;
}bytecode_fn;

ada_declare(bytecode_fn, bytecode_fn_dyn_array)

typedef struct
{
	workspace* ws;
	bytecode_fn_dyn_array fns;
	bytecode bc;
}bytecode_context;

void dump_bytecode(bytecode_context* bcc);
void bytecode_emit_function(bytecode_context* bcc,  bytecode_fn* bcfn,  ast_node* node);
register_id bytecode_emit_expr(bytecode_context* bcc, bytecode_fn* fn,  ast_node* expr);
void bytecode_emit_block(bytecode_context* bcc, bytecode_fn* fn, ast_block* ast_block);
void bytecode_dump_op(bytecode_context* bcc, bytecode_op* op);
u32 bytecode_find_function_id(bytecode_context* bcc, const char* identifier);
void make_bytecode(bytecode_context* bcc);

#endif
