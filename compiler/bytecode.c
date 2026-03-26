#include "bytecode.h"
#include "ast_node.h"
#include <assert.h>
#include <stdio.h>

#define jo_bytecode_stringify_inst_cast_suite(to)\
	jo_stringify_case(jo_bytecode_instr_cast_u8_##to);\
	jo_stringify_case(jo_bytecode_instr_cast_u16_##to);\
	jo_stringify_case(jo_bytecode_instr_cast_u32_##to);\
	jo_stringify_case(jo_bytecode_instr_cast_u64_##to);\
	jo_stringify_case(jo_bytecode_instr_cast_i8_##to);\
	jo_stringify_case(jo_bytecode_instr_cast_i16_##to);\
	jo_stringify_case(jo_bytecode_instr_cast_i32_##to);\
	jo_stringify_case(jo_bytecode_instr_cast_i64_##to);\
	jo_stringify_case(jo_bytecode_instr_cast_f32_##to);\
	jo_stringify_case(jo_bytecode_instr_cast_f64_##to);\
	jo_stringify_case(jo_bytecode_instr_cast_bool_##to)


#define jo_bytecode_stringify_instr_def_suite(op)\
	jo_stringify_case(jo_bytecode_instr_##op##_u8);\
	jo_stringify_case(jo_bytecode_instr_##op##_u16);\
	jo_stringify_case(jo_bytecode_instr_##op##_u32);\
	jo_stringify_case(jo_bytecode_instr_##op##_u64);\
	jo_stringify_case(jo_bytecode_instr_##op##_i8);\
	jo_stringify_case(jo_bytecode_instr_##op##_i16);\
	jo_stringify_case(jo_bytecode_instr_##op##_i32);\
	jo_stringify_case(jo_bytecode_instr_##op##_i64);\
	jo_stringify_case(jo_bytecode_instr_##op##_f32);\
	jo_stringify_case(jo_bytecode_instr_##op##_f64);\
	jo_stringify_case(jo_bytecode_instr_##op##_bool);\


#define jo_bytecode_instrt_binary_case_suite(op)\
	case jo_bytecode_instr_##op##_u8:\
	case jo_bytecode_instr_##op##_u16:\
	case jo_bytecode_instr_##op##_u32:\
	case jo_bytecode_instr_##op##_u64:\
	case jo_bytecode_instr_##op##_i8:\
	case jo_bytecode_instr_##op##_i16:\
	case jo_bytecode_instr_##op##_i32:\
	case jo_bytecode_instr_##op##_i64:\
	case jo_bytecode_instr_##op##_f32:\
	case jo_bytecode_instr_##op##_f64:\
	case jo_bytecode_instr_##op##_bool:


#define jo_bytecode_instr_cast_case_suite(to_type)\
switch (expr->data.expr_as_cast.expr->resolved_type->data.type_primitive)\
{\
	case jo_token_keyword_i64:\
		instr = jo_bytecode_instr_cast_i64_##to_type;\
		break;\
	case jo_token_keyword_u64:\
		instr = jo_bytecode_instr_cast_u64_##to_type;\
		break;\
	case jo_token_keyword_i32:\
		instr = jo_bytecode_instr_cast_i32_##to_type;\
		break;\
	case jo_token_keyword_u32:\
		instr = jo_bytecode_instr_cast_u32_##to_type;\
		break;\
	case jo_token_keyword_i16:\
		instr = jo_bytecode_instr_cast_i16_##to_type;\
		break;\
	case jo_token_keyword_u16:\
		instr = jo_bytecode_instr_cast_u16_##to_type;\
		break;\
	case jo_token_keyword_i8:\
		instr = jo_bytecode_instr_cast_i8_##to_type;\
		break;\
	case jo_token_keyword_u8:\
		instr = jo_bytecode_instr_cast_u8_##to_type;\
		break;\
	case jo_token_keyword_f64:\
		instr = jo_bytecode_instr_cast_f64_##to_type;\
		break;\
	case jo_token_keyword_f32:\
		instr = jo_bytecode_instr_cast_f32_##to_type;\
		break;\
	case jo_token_keyword_bool:\
		instr = jo_bytecode_instr_cast_bool_##to_type;\
		break;\
}

#define jo_bytecode_instr_binary_suite(op)\
switch (expr->resolved_type->data.type_primitive)\
{\
case jo_token_keyword_i64:\
	instr = jo_bytecode_instr_##op##_i64;\
	break;\
case jo_token_keyword_u64:\
	instr = jo_bytecode_instr_##op##_u64;\
	break;\
case jo_token_keyword_i32:\
	instr = jo_bytecode_instr_##op##_i32;\
	break;\
case jo_token_keyword_u32:\
	instr = jo_bytecode_instr_##op##_u32;\
	break;\
case jo_token_keyword_i16:\
	instr = jo_bytecode_instr_##op##_i16;\
	break;\
case jo_token_keyword_u16:\
	instr = jo_bytecode_instr_##op##_u16;\
	break;\
case jo_token_keyword_i8:\
	instr = jo_bytecode_instr_##op##_i8;\
	break;\
case jo_token_keyword_u8:\
	instr = jo_bytecode_instr_##op##_u8;\
	break;\
case jo_token_keyword_f64:\
	instr = jo_bytecode_instr_##op##_f64;\
	break;\
case jo_token_keyword_f32:\
	instr = jo_bytecode_instr_##op##_f32;\
	break;\
case jo_token_keyword_bool:\
	instr = jo_bytecode_instr_##op##_bool;\
	break;\
default:\
	break;\
}


#define jo_bytecode_mov_imm_case(type)\
case jo_ast_type_literal_##type:\
	{\
		jo_register_id out_reg = fn->reg_counter++;\
		jo_bytecode_op op = {0};\
		op.instr = jo_bytecode_instr_mov_imm;\
		op.as.mov_imm.to = out_reg;\
		memcpy(&op.as.mov_imm.value, &expr->data.literal_##type, sizeof(jo_##type));   \
		jo_dyn_array_append(&bcc->bc, op);\
		return out_reg;\
		break;\
	}


bool jo_bytecode_type_is_void(jo_ast_node_t* type_node)
{
	if(!type_node)
	{
		jo_err("unresolved type\n", "");
		return false;
	}

	if(type_node->type != jo_ast_type_type_primitive)
	{
		return false;
	}
	else
	{
		return type_node->data.type_primitive == jo_token_keyword_void;
	}

}

const char* jo_bytecode_instr_to_str(jo_bytecode_instr instr)
{
	switch (instr)
	{
		jo_stringify_case(jo_bytecode_instr_push);
		jo_stringify_case(jo_bytecode_instr_pop);

		jo_stringify_case(jo_bytecode_instr_jmp);
		jo_stringify_case(jo_bytecode_instr_jmp_if);
		jo_stringify_case(jo_bytecode_instr_jmp_if_not);
		jo_stringify_case(jo_bytecode_instr_store);
		jo_stringify_case(jo_bytecode_instr_memcpy);
		jo_stringify_case(jo_bytecode_instr_mov);
		jo_stringify_case(jo_bytecode_instr_mov_imm);

		jo_bytecode_stringify_instr_def_suite(load);
		jo_bytecode_stringify_instr_def_suite(add);
		jo_bytecode_stringify_instr_def_suite(sub);
		jo_bytecode_stringify_instr_def_suite(mul);
		jo_bytecode_stringify_instr_def_suite(div);
		jo_bytecode_stringify_instr_def_suite(cmp_lt);
		jo_bytecode_stringify_instr_def_suite(cmp_lte);
		jo_bytecode_stringify_instr_def_suite(cmp_gt);
		jo_bytecode_stringify_instr_def_suite(cmp_gte);
		jo_bytecode_stringify_instr_def_suite(cmp_eq);
		jo_bytecode_stringify_instr_def_suite(cmp_neq);

		jo_bytecode_stringify_inst_cast_suite(i8);
		jo_bytecode_stringify_inst_cast_suite(u8);
		jo_bytecode_stringify_inst_cast_suite(i16);
		jo_bytecode_stringify_inst_cast_suite(u16);
		jo_bytecode_stringify_inst_cast_suite(i32);
		jo_bytecode_stringify_inst_cast_suite(u32);
		jo_bytecode_stringify_inst_cast_suite(i64);
		jo_bytecode_stringify_inst_cast_suite(u64);
		jo_bytecode_stringify_inst_cast_suite(f32);
		jo_bytecode_stringify_inst_cast_suite(f64);
		jo_bytecode_stringify_inst_cast_suite(bool);

		jo_stringify_case(jo_bytecode_instr_ret);
		jo_stringify_case(jo_bytecode_instr_call);

	default:
		return "invalid bytecode instruction";
		break;
	}
}

void jo_bytecode_dump_op(jo_bytecode_context* bcc, jo_bytecode_op* op)
{
    printf("%-30s ",  jo_bytecode_instr_to_str(op->instr));

	switch (op->instr)
	{
		case jo_bytecode_instr_mov_imm:
			printf("r%u, %#x", op->as.mov_imm.to, op->as.mov_imm.value);
			break;

		case jo_bytecode_instr_jmp:
			printf("%llu", op->as.jmp.offset);
			break;

		case jo_bytecode_instr_jmp_if:
			printf("r%u, %llu", op->as.jmp_if.cond_reg, op->as.jmp_if.offset);
			break;

		case jo_bytecode_instr_jmp_if_not:
			printf("r%u, %llu", op->as.jmp_if_not.cond_reg, op->as.jmp_if_not.offset);
			break;

		case jo_bytecode_instr_mov:
			printf("r%u, r%u", op->as.mov.to, op->as.mov.from);
			break;

		case jo_bytecode_instr_call:
			if(op->as.call.is_void_call)
			{
				printf("%s, args: %u", bcc->fns.data[op->as.call.function_index].label.data, op->as.call.arg_count);
			}
			else
			{
				printf("%s, args: %u -> r%u", bcc->fns.data[op->as.call.function_index].label.data, op->as.call.arg_count, op->as.call.dest);
			}
			break;

		case jo_bytecode_instr_ret:
			if (op->as.ret.is_void) 
			{
				printf("<void>");
			} else {
				printf("r%u", op->as.ret.reg);
			}
			break;

		case jo_bytecode_instr_store:
			printf("[r%u], r%u (size: %u)", op->as.store.to_mem, op->as.store.from_reg, op->as.store.size);
			break;

		case jo_bytecode_instr_memcpy:
			printf("[r%u], [r%u] (size: %u)", op->as.memcpy.to, op->as.memcpy.from, op->as.memcpy.size);
			break;

		jo_bytecode_instrt_binary_case_suite(load)
			printf("r%u, [r%u]", op->as.load.to_reg, op->as.load.from_mem);
			break;

		jo_bytecode_instrt_binary_case_suite(add)
			printf("r%u = r%u + r%u", op->as.binary_op.dest, op->as.binary_op.a, op->as.binary_op.b);
			break;

		jo_bytecode_instrt_binary_case_suite(sub)
			printf("r%u = r%u - r%u", op->as.binary_op.dest, op->as.binary_op.a, op->as.binary_op.b);
			break;

		jo_bytecode_instrt_binary_case_suite(mul)
			printf("r%u = r%u * r%u", op->as.binary_op.dest, op->as.binary_op.a, op->as.binary_op.b);
			break;

		jo_bytecode_instrt_binary_case_suite(div)
			printf("r%u = r%u / r%u", op->as.binary_op.dest, op->as.binary_op.a, op->as.binary_op.b);
			break;

		jo_bytecode_instrt_binary_case_suite(cmp_lt)
			printf("r%u = r%u < r%u", op->as.binary_op.dest, op->as.binary_op.a, op->as.binary_op.b);
			break;

		jo_bytecode_instrt_binary_case_suite(cmp_lte)
			printf("r%u = r%u <= r%u", op->as.binary_op.dest, op->as.binary_op.a, op->as.binary_op.b);
			break;

		jo_bytecode_instrt_binary_case_suite(cmp_gt)
			printf("r%u = r%u > r%u", op->as.binary_op.dest, op->as.binary_op.a, op->as.binary_op.b);
			break;

		jo_bytecode_instrt_binary_case_suite(cmp_gte)
			printf("r%u = r%u >= r%u", op->as.binary_op.dest, op->as.binary_op.a, op->as.binary_op.b);
			break;

		jo_bytecode_instrt_binary_case_suite(cmp_eq)
			printf("r%u = r%u == r%u", op->as.binary_op.dest, op->as.binary_op.a, op->as.binary_op.b);
			break;

		jo_bytecode_instrt_binary_case_suite(cmp_neq)
			printf("r%u = r%u !	= r%u", op->as.binary_op.dest, op->as.binary_op.a, op->as.binary_op.b);
			break;

		default:
		    printf("r%u -> r%u", op->as.cast.target, op->as.cast.dest);
			break;
	}

	printf("\n");
}

jo_u32 jo_bytecode_find_function(jo_bytecode_context* bcc, const char* identifier)
{
	jo_dyn_array_iter(&bcc->fns, it,
		{
			if(strcmp(bcc->fns.data[it].label.data, identifier) == 0)
			{
				return it;
			}
		}
	);
}

jo_register_id jo_bytecode_emit_expr(jo_bytecode_context* bcc, jo_bytecode_fn* fn, jo_ast_node_t* expr)
{
	jo_ast_node_t* decl_identifier_node = expr->data.decl.identifier;

	switch (expr->type)
	{
	case jo_ast_type_decl:

		if(expr->resolved_type->type == jo_ast_type_type_fn)
		{
			jo_bytecode_fn bc_fn = {0};
            bc_fn.label = jo_string_from(expr->data.decl.identifier->data.identifier.data);
            bc_fn.entry_ip = -1; // patched in later
            jo_dyn_array_append(&bcc->fns, bc_fn);

			jo_bytecode_emit_function(bcc, &bcc->fns.data[bcc->fns.occupied - 1], expr);
		}
		else
		{
			if(expr->data.decl.initialize_expression)
			{
				jo_bytecode_op mov = {0};
				mov.instr = jo_bytecode_instr_mov;
				jo_register_id from_reg = jo_bytecode_emit_expr(bcc, fn, expr->data.decl.initialize_expression);
				jo_register_id to_reg = fn->reg_counter++;
				
				mov.as.mov.from = from_reg;
				mov.as.mov.to = to_reg;
				jo_dyn_array_append(&bcc->bc, mov);

				decl_identifier_node->resolved_symbol->location = mov.as.mov.to;
			}
			else
			{
				decl_identifier_node->resolved_symbol->location = fn->reg_counter++;
				
			}
			
			return decl_identifier_node->resolved_symbol->location;
		}

		break;

	case jo_ast_type_identifier:
		{	
			if(!expr->resolved_symbol)
			{
				jo_err("undeclared identifier %s", expr->data.identifier.data);
			}
			return expr->resolved_symbol->location;
			break;
		}

	case jo_ast_type_expr_as_cast:
		{
            jo_bytecode_op op = {0};
			jo_register_id target_reg = jo_bytecode_emit_expr(bcc, fn, expr->data.expr_as_cast.expr);           
			jo_register_id dest_reg = fn->reg_counter++;
			
			op.as.cast.target = target_reg;
			op.as.cast.dest = dest_reg;
			jo_bytecode_instr instr = {0};
			switch (expr->data.expr_as_cast.to_type->data.type_primitive)
			{
				case jo_token_keyword_i8:
					jo_bytecode_instr_cast_case_suite(i8);
					break;
				case jo_token_keyword_u8:
					jo_bytecode_instr_cast_case_suite(u8);
					break;

				case jo_token_keyword_i16:
					jo_bytecode_instr_cast_case_suite(i16);
					break;
				case jo_token_keyword_u16:
					jo_bytecode_instr_cast_case_suite(u16);
					break;

				case jo_token_keyword_i32:
					jo_bytecode_instr_cast_case_suite(i32);
					break;
				case jo_token_keyword_u32:
					jo_bytecode_instr_cast_case_suite(u32);
					break;

				case jo_token_keyword_i64:
					jo_bytecode_instr_cast_case_suite(i64);
					break;
				case jo_token_keyword_u64:
					jo_bytecode_instr_cast_case_suite(u64);
					break;

				case jo_token_keyword_f32:
					jo_bytecode_instr_cast_case_suite(f32);
					break;
				case jo_token_keyword_f64:
					jo_bytecode_instr_cast_case_suite(f64);
					break;

				case jo_token_keyword_bool:
					jo_bytecode_instr_cast_case_suite(bool);
					break;

			default:

				break;
			}

			op.instr = instr;
            jo_dyn_array_append(&bcc->bc, op);
			return op.as.cast.dest;
			break;
		}

	case jo_ast_type_expr_op_binary:
		{
            jo_register_id left_reg = jo_bytecode_emit_expr(bcc, fn, expr->data.expr_op_binary.left_expression);
            jo_register_id right_reg = jo_bytecode_emit_expr(bcc, fn, expr->data.expr_op_binary.right_expression);

            jo_bytecode_op op = {0};
			jo_bytecode_instr instr = 0;

			jo_register_id dest_reg = fn->reg_counter++;

			if(!expr->resolved_type){ assert(0); }

			if(expr->resolved_type->type == jo_ast_type_type_primitive)
			{
				switch (expr->data.expr_op_binary.operator_type)
				{
					case jo_token_plus_equals:
						fn->reg_counter--;
						dest_reg = left_reg;
					case jo_token_plus:
						jo_bytecode_instr_binary_suite(add);
						break;

					case jo_token_star_equals:
						fn->reg_counter--;
						dest_reg = left_reg;
					case jo_token_star:
						jo_bytecode_instr_binary_suite(mul);
						break;

					case jo_token_slash_equals:
						fn->reg_counter--;
						dest_reg = left_reg;
					case jo_token_slash:
						jo_bytecode_instr_binary_suite(div);
						break;

					case jo_token_minus_equals:
						fn->reg_counter--;
						dest_reg = left_reg;
					case jo_token_minus:
						jo_bytecode_instr_binary_suite(sub);
						break;

					case jo_token_greater:
						jo_bytecode_instr_binary_suite(cmp_gt);
						break;
					case jo_token_greater_equals:
						jo_bytecode_instr_binary_suite(cmp_gte);
						break;
					case jo_token_less:
						jo_bytecode_instr_binary_suite(cmp_lt);
						break;
					case jo_token_less_equals:
						jo_bytecode_instr_binary_suite(cmp_lte);
						break;
					case jo_token_double_equals:
						jo_bytecode_instr_binary_suite(cmp_eq);
						break;
					case jo_token_not_equals:
						jo_bytecode_instr_binary_suite(cmp_neq);
						break;

					case jo_token_equals:
					{
						fn->reg_counter--;
						dest_reg = left_reg;
						jo_bytecode_op mov = {0};
						mov.instr = jo_bytecode_instr_mov;
						mov.as.mov.to = left_reg;
						mov.as.mov.from = right_reg;
						jo_dyn_array_append(&bcc->bc, mov);
						return left_reg;
						break;
					}
					default:
						break;
				}
			}

            op.instr = instr;
            op.as.binary_op.dest = dest_reg;
            op.as.binary_op.a = left_reg;
            op.as.binary_op.b = right_reg;

            jo_dyn_array_append(&bcc->bc, op);
            return dest_reg;
		}
		break;

	jo_bytecode_mov_imm_case(i8);
	jo_bytecode_mov_imm_case(u8);
	jo_bytecode_mov_imm_case(i16);
	jo_bytecode_mov_imm_case(u16);
	jo_bytecode_mov_imm_case(i32);
	jo_bytecode_mov_imm_case(u32);
	jo_bytecode_mov_imm_case(i64);
	jo_bytecode_mov_imm_case(u64);
	jo_bytecode_mov_imm_case(f32);
	jo_bytecode_mov_imm_case(f64);

	jo_bytecode_mov_imm_case(bool);

	case jo_ast_type_expr_op_call:
	{
		bool void_call = jo_bytecode_type_is_void(expr->resolved_type);

		jo_u32 arg_count = expr->data.expr_op_call.arguments.occupied;

		jo_register_id dest_reg = void_call ? jo_null_register : fn->reg_counter++;

		// @Important: this adds hard limit on ammount of functiona arguments
		jo_register_id args_ids[128];
		jo_u32 args_ids_counter = 0;

		for (jo_u32 i = 0; i < arg_count; i++)
		{
			jo_ast_node_t* arg_expr = expr->data.expr_op_call.arguments.data[i];
			args_ids[args_ids_counter++] = jo_bytecode_emit_expr(bcc, fn, arg_expr);
		}

		jo_bytecode_op op = {0};
		op.instr = jo_bytecode_instr_call;
		op.as.call.arg_count = arg_count;
		op.as.call.is_void_call = void_call;
		op.as.call.dest = dest_reg;
		op.as.call.first_arg = fn->reg_counter;
		op.as.call.function_index = jo_bytecode_find_function(bcc, expr->data.expr_op_call.target->data.identifier.data);

		for(jo_u32 i = 0; i < args_ids_counter; i++)
		{
			jo_bytecode_op op = {0};
			op.instr = jo_bytecode_instr_mov;
			op.as.mov.to = fn->reg_counter++;
			op.as.mov.from = args_ids[i];
			jo_dyn_array_append(&bcc->bc, op);
		}

		jo_dyn_array_append(&bcc->bc, op);


		return dest_reg;
	}
	default:
		break;
	}
}

void jo_bytecode_emit_stmt(jo_bytecode_context* bcc,  jo_bytecode_fn* fn, jo_ast_node_t* stmt_node)
{
	switch (stmt_node->type)
	{
		case jo_ast_type_block:
			{
				jo_bytecode_emit_block(bcc, fn,  &stmt_node->data.block);
				break;

			}
		case jo_ast_type_stmt_return:
			{
				jo_bytecode_op ret_op = {0};
				ret_op.instr = jo_bytecode_instr_ret;				
				if(stmt_node->data.stmt_return.expression)
				{
					ret_op.as.ret.reg = jo_bytecode_emit_expr(bcc, fn,  stmt_node->data.stmt_return.expression);
				}
				else
				{
					ret_op.as.ret.is_void = true;
				}
				jo_dyn_array_append(&bcc->bc, ret_op);
				break;
			}
		case jo_ast_type_stmt_ifelse:
			{
				jo_bytecode_op jmp_in_not_op = {0};
				jmp_in_not_op.instr = jo_bytecode_instr_jmp_if_not;
				jmp_in_not_op.as.jmp_if_not.cond_reg = jo_bytecode_emit_expr(bcc, fn, stmt_node->data.stmt_ifelse.condition);
				jmp_in_not_op.as.jmp_if_not.offset = -1; // patched later after we generate the {success} block to know how much instrucitons to jump over
				jo_dyn_array_append(&bcc->bc, jmp_in_not_op);
				jo_u32 cond_jmp_instr_id = bcc->bc.occupied - 1;

				jo_bytecode_emit_block(bcc, fn, &stmt_node->data.stmt_ifelse.true_block->data.block);

				jo_bytecode_op jmp_out_op = {0};
				jmp_out_op.instr = jo_bytecode_instr_jmp;
				jmp_out_op.as.jmp.offset = -1; // patched later after we generate the {else} block to know how much instrucitons to jump over
				jo_dyn_array_append(&bcc->bc, jmp_out_op);
				jo_u32 if_out_jmp_instr_id = bcc->bc.occupied - 1;

				bcc->bc.data[cond_jmp_instr_id].as.jmp_if_not.offset = bcc->bc.occupied - cond_jmp_instr_id;

				if(stmt_node->data.stmt_ifelse.tail_stmt_if)
				{
					jo_bytecode_emit_stmt(bcc, fn,  stmt_node->data.stmt_ifelse.tail_stmt_if);
				}
				else if(stmt_node->data.stmt_ifelse.false_block)
				{
					jo_bytecode_emit_block(bcc, fn,  &stmt_node->data.stmt_ifelse.false_block->data.block);
				}

				bcc->bc.data[if_out_jmp_instr_id].as.jmp.offset = bcc->bc.occupied - if_out_jmp_instr_id;

				break;
			}
		case jo_ast_type_stmt_expr:
			{
				jo_bytecode_emit_expr(bcc, fn, stmt_node->data.stmt_expr.expr);
				break;
			}
	default:
		printf("%s\n", jo_ast_node_type_to_stirng(stmt_node->type));
		assert(0);
		break;
	}
}

void jo_bytecode_emit_block(jo_bytecode_context* bcc, jo_bytecode_fn* fn, jo_ast_block* ast_block)
{
	for(jo_uz i = 0; i < ast_block->statements.occupied; i++)
	{
		jo_ast_node_t* stmt = ast_block->statements.data[i];

		jo_bytecode_emit_stmt(bcc, fn,  stmt);
	}
}

void jo_bytecode_emit_function(jo_bytecode_context* bcc, jo_bytecode_fn* bcfn, jo_ast_node_t* node)
{
    jo_ast_decl* decl_node = &node->data.decl;
    jo_ast_literal_fn* literal_fn = &decl_node->initialize_expression->data.literal_fn;

	if(literal_fn->intrinsic) return;
	
	for(jo_uz param_id = 0; param_id < literal_fn->parameters.occupied; param_id++)
	{
		literal_fn->parameters.data[param_id]->resolved_symbol->location = bcfn->reg_counter++;
	}
		
    jo_bytecode_emit_block(bcc, bcfn, &literal_fn->block->data.block);
}

jo_bytecode_context jo_make_bytecode(jo_ast_module* module)
{
	jo_bytecode_context bcc = {0};

	jo_dyn_array_iter(&module->content, it, {
        jo_ast_node_t* node = module->content.data[it];
        if(node->data.decl.initialize_expression->resolved_type->type == jo_ast_type_type_fn) {
            jo_bytecode_fn bc_fn = {0};
            bc_fn.label = jo_string_from(node->data.decl.identifier->data.identifier.data);
            bc_fn.entry_ip = -1; // patched in later
            jo_dyn_array_append(&bcc.fns, bc_fn);
        }
    });

	jo_dyn_array_iter(&module->content, it, {
        jo_ast_node_t* node = module->content.data[it];
        if(node->data.decl.initialize_expression->resolved_type->type == jo_ast_type_type_fn) {
            jo_u32 fn_index = jo_bytecode_find_function(&bcc, node->data.decl.identifier->data.identifier.data);

            bcc.fns.data[fn_index].entry_ip = bcc.bc.occupied;

            jo_bytecode_emit_function(&bcc, &bcc.fns.data[fn_index], node);
        }
    });

	return bcc;
}
