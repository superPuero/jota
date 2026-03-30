#include "bytecode.h"
#include "ast_node.h"
#include <assert.h>
#include <stdio.h>
#include "workspace.h"

#define jo_bc_cast_case_suite(to_type)\
switch (expr->data.expr_as_cast.expr->resolved_type->data.type_primitive)\
{\
	case jo_tok_i64:\
		instr = jo_bc_cast_i64_##to_type;\
		break;\
	case jo_tok_u64:\
		instr = jo_bc_cast_u64_##to_type;\
		break;\
	case jo_tok_i32:\
		instr = jo_bc_cast_i32_##to_type;\
		break;\
	case jo_tok_u32:\
		instr = jo_bc_cast_u32_##to_type;\
		break;\
	case jo_tok_i16:\
		instr = jo_bc_cast_i16_##to_type;\
		break;\
	case jo_tok_u16:\
		instr = jo_bc_cast_u16_##to_type;\
		break;\
	case jo_tok_i8:\
		instr = jo_bc_cast_i8_##to_type;\
		break;\
	case jo_tok_u8:\
		instr = jo_bc_cast_u8_##to_type;\
		break;\
	case jo_tok_f64:\
		instr = jo_bc_cast_f64_##to_type;\
		break;\
	case jo_tok_f32:\
		instr = jo_bc_cast_f32_##to_type;\
		break;\
	case jo_tok_bool:\
		instr = jo_bc_cast_bool_##to_type;\
		break;\
	default:\
		assert(0);\
		break;\
}

#define jo_bc_binary_suite(op)\
switch (expr->resolved_type->data.type_primitive)\
{\
case jo_tok_i64:\
	instr = jo_bc_##op##_i64;\
	break;\
case jo_tok_u64:\
	instr = jo_bc_##op##_u64;\
	break;\
case jo_tok_i32:\
	instr = jo_bc_##op##_i32;\
	break;\
case jo_tok_u32:\
	instr = jo_bc_##op##_u32;\
	break;\
case jo_tok_i16:\
	instr = jo_bc_##op##_i16;\
	break;\
case jo_tok_u16:\
	instr = jo_bc_##op##_u16;\
	break;\
case jo_tok_i8:\
	instr = jo_bc_##op##_i8;\
	break;\
case jo_tok_u8:\
	instr = jo_bc_##op##_u8;\
	break;\
case jo_tok_f64:\
	instr = jo_bc_##op##_f64;\
	break;\
case jo_tok_f32:\
	instr = jo_bc_##op##_f32;\
	break;\
case jo_tok_bool:\
	instr = jo_bc_##op##_bool;\
	break;\
default:\
	break;\
}

#define jo_bc_literal_mov_imm_case(type)\
case jo_ast_type_literal_##type:\
	{\
		jo_register_id out_reg = fn->reg_counter++;\
		jo_bytecode_op op = {0};\
		op.instr = jo_bc_mov_imm;\
		op.as.mov_imm.to = out_reg;\
		op.as.mov_imm.size = 8;\
		memcpy(&op.as.mov_imm.value, &expr->data.literal_##type, sizeof(jo_##type));   \
		jo_ada_append(&bcc->ws->arena,&bcc->bc, op);\
		return out_reg;\
		break;\
	}

const char* intrinsic_map[] = {
	"print"
};

void jo_dump_bytecode(jo_bytecode_context* bcc)	
{
	jo_uz inst = 0;
	jo_ada_foreach(&bcc->bc)
	{
		jo_bytecode_op* op = bcc->bc.it;

		jo_ada_foreach(&bcc->fns)
		{
			if(inst == bcc->fns.it->entry_ip)
			{
				printf("%.*s (registers: %u)\n", bcc->fns.it->label.len, bcc->fns.it->label.data, bcc->fns.it->reg_counter);	
			}
		}

		printf("%4zu: ", inst);
		jo_bytecode_dump_op(bcc, op);				
		inst++;
	}
}

bool jo_bytecode_type_is_void(jo_ast_node* type_node)
{
	if(!type_node)
	{
		assert(0 && "unresolved type\n");
		return false;
	}

	if(type_node->type != jo_ast_type_type_primitive)
	{
		return false;
	}
	else
	{
		return type_node->data.type_primitive == jo_tok_void;
	}

}

const char* jo_bc_to_str(jo_bc_instr instr)
{
	switch (instr)
	{
	#define X(opcode) jo_stringify_case(jo_bc_##opcode);
		jo_bc_basic_instr_list
	#undef X

	#define XYX(opcode, t, op)  jo_stringify_case(jo_bc_##opcode##_##t);
	#define X(t) jo_bc_binary_primitive_instr_list(t)
		jo_tok_numerical_type_primitive_list
	#undef X
	#undef XYX

	#define XY(t1, t2)  jo_stringify_case(jo_bc_cast_##t1##_##t2);
	#define X(t) jo_bc_inst_cast_list(t)
		jo_tok_numerical_type_primitive_list
	#undef X
	#undef XY	
	default:
		return "invalid bytecode instruction";
		break;
	}
}

void jo_bytecode_dump_op(jo_bytecode_context* bcc, jo_bytecode_op* op)
{
    printf("%-30s ",  jo_bc_to_str(op->instr));

	switch (op->instr)
	{
		case jo_bc_mov_imm:
			printf("r%u, %llx", op->as.mov_imm.to, op->as.mov_imm.value);
			break;

		case jo_bc_jmp:
			printf("%u", op->as.jmp.offset);
			break;

		case jo_bc_jmp_if:
			printf("r%u, %u", op->as.jmp_if.cond_reg, op->as.jmp_if.offset);
			break;

		case jo_bc_jmp_if_not:
			printf("r%u, %u", op->as.jmp_if_not.cond_reg, op->as.jmp_if_not.offset);
			break;

		case jo_bc_mov:
			printf("r%u, r%u", op->as.mov.to, op->as.mov.from);
			break;

		case jo_bc_call:
			if(op->as.call.is_void_call)
			{
				printf("%.*s, args: %u", bcc->fns.data[op->as.call.function_index].label.len, bcc->fns.data[op->as.call.function_index].label.data, op->as.call.arg_count);
			}
			else
			{
				printf("%.*s, args: %u -> r%u", bcc->fns.data[op->as.call.function_index].label.len, bcc->fns.data[op->as.call.function_index].label.data, op->as.call.arg_count, op->as.call.dest);
			}
			break;

		case jo_bc_ret:
			if (op->as.ret.is_void) 
			{
				printf("<void>");
			} else {
				printf("r%u", op->as.ret.reg);
			}
			break;

		default:
		    printf("r%u -> r%u", op->as.cast.target, op->as.cast.dest);
			break;
	}

	printf("\n");
}

jo_u32 jo_bytecode_find_function_id(jo_bytecode_context* bcc, const char* identifier)
{
	jo_u32 i = 0;
	jo_ada_foreach(&bcc->fns)
	{
		if(strncmp(bcc->fns.it->label.data, identifier, bcc->fns.it->label.len) == 0)
		{
			return i;
		}

		i++;
	}

	return 	UINT32_MAX;
}

jo_register_id jo_bytecode_emit_expr(jo_bytecode_context* bcc, jo_bytecode_fn* fn, jo_ast_node* expr)
{
	jo_ast_node* decl_identifier_node = expr->data.decl.identifier;

	switch (expr->type)
	{
	case jo_ast_type_decl:		
		if(expr->resolved_type->type == jo_ast_type_type_fn)
		{
			jo_bytecode_fn bc_fn = {0};
            bc_fn.label = expr->data.decl.identifier->data.identifier;
            bc_fn.entry_ip = -1; // patched in later
            jo_ada_append(&bcc->ws->arena, &bcc->fns, bc_fn);

			jo_bytecode_emit_function(bcc, &bcc->fns.data[bcc->fns.occupied - 1], expr);
		}
		else
		{
			if(expr->data.decl.initialize_expression)
			{
				jo_bytecode_op mov = {0};
				mov.instr = jo_bc_mov;
				jo_register_id from_reg = jo_bytecode_emit_expr(bcc, fn, expr->data.decl.initialize_expression);
				jo_register_id to_reg = fn->reg_counter++;
				
				mov.as.mov.from = from_reg;
				mov.as.mov.to = to_reg;
				jo_ada_append(&bcc->ws->arena, &bcc->bc, mov);

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
				printf("undeclared identifier %s", expr->data.identifier.data);
				assert(0);
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
			jo_bc_instr instr = {0};

			jo_tok dest_type = expr->data.expr_as_cast.to_type->data.type_primitive;

			if(jo_tok_is_numerical(dest_type))
			{
				switch (dest_type)
				{
					#define X(t) case jo_tok_##t :\
						jo_bc_cast_case_suite(t);\
						break;

						jo_tok_numerical_type_primitive_list
					#undef X
	
					default:
						assert(0);
						break;
				}
	
				op.instr = instr;
				jo_ada_append(&bcc->ws->arena, &bcc->bc, op);
				return op.as.cast.dest;
			}

			break;
		}

	case jo_ast_type_expr_op_binary:
		{
            jo_register_id left_reg = jo_bytecode_emit_expr(bcc, fn, expr->data.expr_op_binary.left_expression);
            jo_register_id right_reg = jo_bytecode_emit_expr(bcc, fn, expr->data.expr_op_binary.right_expression);

            jo_bytecode_op op = {0};
			jo_bc_instr instr = 0;

			jo_register_id dest_reg = fn->reg_counter++;

			if(!expr->resolved_type){ assert(0); }

			if(expr->resolved_type->type == jo_ast_type_type_primitive)
			{
				switch (expr->data.expr_op_binary.operator_type)
				{
					// 	fn->reg_counter--;
					// 	dest_reg = left_reg;

					#define XYX(opcode, t, op) case jo_tok_##opcode :\
						jo_bc_binary_suite(opcode);\
						break;
						jo_bc_binary_primitive_instr_list(t)
					#undef XYX

					case jo_tok_equals:
					{
						fn->reg_counter--;
						dest_reg = left_reg;
						jo_bytecode_op mov = {0};
						mov.instr = jo_bc_mov;
						mov.as.mov.to = left_reg;
						mov.as.mov.from = right_reg;
						jo_ada_append(&bcc->ws->arena, &bcc->bc, mov);
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

            jo_ada_append(&bcc->ws->arena,&bcc->bc, op);
            return dest_reg;
		}
		break;

	jo_bc_literal_mov_imm_case(i64);
	jo_bc_literal_mov_imm_case(u64);
	jo_bc_literal_mov_imm_case(f64);
	jo_bc_literal_mov_imm_case(bool);

	case jo_ast_type_literal_string:
	{
		jo_register_id base_addr_reg = fn->reg_counter++;
		jo_uz len = expr->data.literal_string.len;

		jo_bytecode_op get_sp_op = {0};
		get_sp_op.instr = jo_bc_get_sp;
		get_sp_op.as.get_sp.to = base_addr_reg;

		jo_ada_append(&bcc->ws->arena, &bcc->bc, get_sp_op);

		jo_bytecode_op alloc_op = {0};
		alloc_op.instr = jo_bc_push; 
		alloc_op.as.push.offset = len + 1; 
		jo_ada_append(&bcc->ws->arena, &bcc->bc, alloc_op);


		for(jo_uz i = 0; i <= len; i++)
		{
			jo_register_id chunk_reg = fn->reg_counter++;	

			jo_bytecode_op mov_imm = {0};
			mov_imm.instr = jo_bc_mov_imm;
			mov_imm.as.mov_imm.to = chunk_reg;
			mov_imm.as.mov_imm.value = i==len ? '\0' : expr->data.literal_string.data[i];
			mov_imm.as.mov_imm.size = 1;	
			jo_ada_append(&bcc->ws->arena, &bcc->bc, mov_imm);
			
			jo_bytecode_op store_op = {0};
			store_op.instr = jo_bc_store; 
			store_op.as.store.from = chunk_reg;
			store_op.as.store.to_addr = base_addr_reg;
			store_op.as.store.size = 1;
			store_op.as.store.offset = i;

			jo_ada_append(&bcc->ws->arena, &bcc->bc, store_op);		
		}   

		return base_addr_reg;
	}


	case jo_ast_type_expr_op_call:
	{
		bool intrsinsic = 0;

		for(jo_uz i = 0; i < sizeof(intrinsic_map)/sizeof(intrinsic_map[0]); i++)
		{
			if(strncmp(intrinsic_map[i], expr->data.expr_op_call.target->data.identifier.data, strlen(intrinsic_map[i])) == 0)
			{
				intrsinsic = true;
			}
		}

		bool void_call = jo_bytecode_type_is_void(expr->resolved_type);

		jo_u32 arg_count = expr->data.expr_op_call.arguments.occupied;

		jo_register_id dest_reg = void_call ? jo_null_register : fn->reg_counter++;

		// @Important: this adds hard limit on ammount of functiona arguments
		jo_register_id args_ids[128];
		jo_u32 args_ids_counter = 0;

		for (jo_u32 i = 0; i < arg_count; i++)
		{
			jo_ast_node* arg_expr = expr->data.expr_op_call.arguments.data[i];
			args_ids[args_ids_counter++] = jo_bytecode_emit_expr(bcc, fn, arg_expr);
		}

		jo_bytecode_op op = {0};
		op.instr = jo_bc_call;
		op.as.call.arg_count = arg_count;
		op.as.call.is_void_call = void_call;
		op.as.call.dest = dest_reg;
		op.as.call.intrinsic = intrsinsic;
		op.as.call.first_arg = fn->reg_counter;
		op.as.call.function_index = jo_bytecode_find_function_id(bcc, expr->data.expr_op_call.target->data.identifier.data);

		if(op.as.call.function_index == UINT32_MAX)
		{
			assert(0);
		}

		for(jo_u32 i = 0; i < args_ids_counter; i++)
		{
			jo_bytecode_op op = {0};
			op.instr = jo_bc_mov;
			op.as.mov.to = fn->reg_counter++;
			op.as.mov.from = args_ids[i];
			jo_ada_append(&bcc->ws->arena,&bcc->bc, op);
		}

		jo_ada_append(&bcc->ws->arena,&bcc->bc, op);


		return dest_reg;
	}
	default:
		assert(0);
		break;
	}

	return jo_null_register;
}

void jo_bytecode_emit_stmt(jo_bytecode_context* bcc,  jo_bytecode_fn* fn, jo_ast_node* stmt_node)
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
				ret_op.instr = jo_bc_ret;				
				if(stmt_node->data.stmt_return.expression)
				{
					ret_op.as.ret.reg = jo_bytecode_emit_expr(bcc, fn,  stmt_node->data.stmt_return.expression);
				}
				else
				{
					ret_op.as.ret.is_void = true;
				}
				jo_ada_append(&bcc->ws->arena,&bcc->bc, ret_op);
				break;
			}
		case jo_ast_type_stmt_ifelse:
			{
				jo_bytecode_op jmp_in_not_op = {0};
				jmp_in_not_op.instr = jo_bc_jmp_if_not;
				jmp_in_not_op.as.jmp_if_not.cond_reg = jo_bytecode_emit_expr(bcc, fn, stmt_node->data.stmt_ifelse.condition);
				jmp_in_not_op.as.jmp_if_not.offset = -1; // patched later after we generate the {success} block to know how much instrucitons to jump over
				jo_ada_append(&bcc->ws->arena,&bcc->bc, jmp_in_not_op);
				jo_u32 cond_jmp_instr_id = bcc->bc.occupied - 1;

				jo_bytecode_emit_block(bcc, fn, &stmt_node->data.stmt_ifelse.true_block->data.block);

				jo_bytecode_op jmp_out_op = {0};
				jmp_out_op.instr = jo_bc_jmp;
				jmp_out_op.as.jmp.offset = -1; // patched later after we generate the {else} block to know how much instrucitons to jump over
				jo_ada_append(&bcc->ws->arena,&bcc->bc, jmp_out_op);
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
		jo_ast_node* stmt = ast_block->statements.data[i];

		jo_bytecode_emit_stmt(bcc, fn,  stmt);
	}
}

void jo_bytecode_emit_function(jo_bytecode_context* bcc, jo_bytecode_fn* bcfn, jo_ast_node* node)
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

void jo_make_bytecode(jo_bytecode_context* bcc)
{
	jo_ada_foreach_named(&bcc->ws->loaded_modules, i)
	{
		jo_ada_foreach_named(&bcc->ws->loaded_modules.it->file_node->data.file.content, j)
		{
			jo_ast_node* node = (*bcc->ws->loaded_modules.it->file_node->data.file.content.it);
			if(node->data.decl.initialize_expression->resolved_type->type == jo_ast_type_type_fn) {
				jo_bytecode_fn bc_fn = {0};
				bc_fn.label = node->data.decl.identifier->data.identifier;
				bc_fn.entry_ip = -1; // patched in later
				jo_ada_append(&bcc->ws->arena, &bcc->fns, bc_fn);
			}
		}
    }

	jo_ada_foreach_named(&bcc->ws->loaded_modules, i)
	{
		jo_ada_foreach_named(&bcc->ws->loaded_modules.it->file_node->data.file.content, j)
		{
			jo_ast_node* node = (*bcc->ws->loaded_modules.it->file_node->data.file.content.it);
			if(node->data.decl.initialize_expression->resolved_type->type == jo_ast_type_type_fn) 
			{
				jo_u32 fn_index = jo_bytecode_find_function_id(bcc, node->data.decl.identifier->data.identifier.data);

				if(fn_index == UINT32_MAX)
				{
					assert(0);
				}

				bcc->fns.data[fn_index].entry_ip = bcc->bc.occupied;

				jo_bytecode_emit_function(bcc, &bcc->fns.data[fn_index], node);
			}
		}
	}
}
