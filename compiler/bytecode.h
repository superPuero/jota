#ifndef jota_bytecode
#define jota_bytecode

#include "../core/core.h"
#include "ast_node.h"
#include "symbol.h"

#define jo_bytecode_inst_cast_suite(to)\
	jo_bytecode_instr_cast_u8_##to,\
	jo_bytecode_instr_cast_u16_##to,\
	jo_bytecode_instr_cast_u32_##to,\
	jo_bytecode_instr_cast_u64_##to,\
	jo_bytecode_instr_cast_i8_##to,\
	jo_bytecode_instr_cast_i16_##to,\
	jo_bytecode_instr_cast_i32_##to,\
	jo_bytecode_instr_cast_i64_##to,\
	jo_bytecode_instr_cast_f32_##to,\
	jo_bytecode_instr_cast_f64_##to,\
	jo_bytecode_instr_cast_bool_##to\

#define jo_bytecode_instr_def_suite(op)\
	jo_bytecode_instr_##op##_u8,\
	jo_bytecode_instr_##op##_u16,\
	jo_bytecode_instr_##op##_u32,\
	jo_bytecode_instr_##op##_u64,\
	jo_bytecode_instr_##op##_i8,\
	jo_bytecode_instr_##op##_i16,\
	jo_bytecode_instr_##op##_i32,\
	jo_bytecode_instr_##op##_i64,\
	jo_bytecode_instr_##op##_f32,\
	jo_bytecode_instr_##op##_f64,\
	jo_bytecode_instr_##op##_bool\


typedef enum
{
	jo_bytecode_instr_push,
	jo_bytecode_instr_pop,

	jo_bytecode_instr_jmp,
	jo_bytecode_instr_jmp_if,
	jo_bytecode_instr_jmp_if_not,

	jo_bytecode_instr_store,

	jo_bytecode_instr_memcpy,

	jo_bytecode_instr_mov,
	jo_bytecode_instr_mov_imm,

	jo_bytecode_instr_def_suite(load),
	jo_bytecode_instr_def_suite(add),
	jo_bytecode_instr_def_suite(sub),
	jo_bytecode_instr_def_suite(mul),
	jo_bytecode_instr_def_suite(div),
	jo_bytecode_instr_def_suite(cmp_lt),
	jo_bytecode_instr_def_suite(cmp_lte),
	jo_bytecode_instr_def_suite(cmp_gt),
	jo_bytecode_instr_def_suite(cmp_gte),
	jo_bytecode_instr_def_suite(cmp_eq),
	jo_bytecode_instr_def_suite(cmp_neq),

	jo_bytecode_inst_cast_suite(i8),
	jo_bytecode_inst_cast_suite(u8),

	jo_bytecode_inst_cast_suite(i16),
	jo_bytecode_inst_cast_suite(u16),

	jo_bytecode_inst_cast_suite(i32),
	jo_bytecode_inst_cast_suite(u32),

	jo_bytecode_inst_cast_suite(i64),
	jo_bytecode_inst_cast_suite(u64),

	jo_bytecode_inst_cast_suite(f32),
	jo_bytecode_inst_cast_suite(f64),

	jo_bytecode_inst_cast_suite(bool),

	jo_bytecode_instr_ret,
	jo_bytecode_instr_call
}jo_bytecode_instr;

const char* jo_bytecode_instr_to_str(jo_bytecode_instr instr);

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
	jo_bytecode_instr instr;

	union
	{
		struct
		{
			jo_register_id target;
			jo_register_id dest;
		} cast;


		struct
		{
            jo_register_id to_reg;
            jo_register_id from_mem;
		} load;

		struct
		{
            jo_register_id to_mem;
            jo_register_id from_reg;
    		jo_u32 size;
		} store;

		struct
		{
            jo_register_id start_reg;
    		jo_u32 size;
		} clear;

		struct
		{
            jo_register_id to;
    		jo_register_id from;
    		jo_u32 size;
		} memcpy;


		struct
		{
            jo_register_id to;
			jo_value64 value;
		} mov_imm;

		struct
		{
            jo_register_id from;
            jo_register_id to;
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
			bool is_void_call;
		}call;

		struct
		{
			jo_register_id reg;
			bool is_void;
		}ret;


    } as;
}jo_bytecode_op;

jo_decl_dyn_array_named(jo_bytecode_op, jo_bytecode);

typedef struct
{
	jo_string label;
	jo_u32 entry_ip;
	jo_register_id reg_counter;
}jo_bytecode_fn;

jo_decl_dyn_array_named(jo_bytecode_fn, jo_bytecode_fn_dyn_array)

typedef struct
{
	jo_bytecode_fn_dyn_array fns;
	jo_bytecode bc;
}jo_bytecode_context;

void jo_bytecode_emit_function(jo_bytecode_context* bcc,  jo_bytecode_fn* bcfn,  jo_ast_node_t* node);
jo_register_id jo_bytecode_emit_expr(jo_bytecode_context* bcc, jo_bytecode_fn* fn,  jo_ast_node_t* expr);
void jo_bytecode_emit_block(jo_bytecode_context* bcc, jo_bytecode_fn* fn, jo_ast_block* ast_block);
void jo_bytecode_dump_op(jo_bytecode_context* bcc, jo_bytecode_op* op);
jo_u32 jo_bytecode_find_function(jo_bytecode_context* bcc, const char* identifier);
jo_bytecode_context jo_make_bytecode(jo_ast_module* module);

#endif
