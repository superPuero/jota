#include "vm.h"

#define binary_op_case(operation, type)\
*(type*)(vm->registers + current_frame->base_register + op.as.binary_op.dest) = \
                    (*(type*)(vm->registers + current_frame->base_register + op.as.binary_op.a) operation \
                    *(type*)(vm->registers + current_frame->base_register + op.as.binary_op.b))

#define cast_case(to_type, from_type)\
*(to_type*)(vm->registers + current_frame->base_register + op.as.cast.dest) = \
                   (to_type)*(from_type*)(vm->registers + current_frame->base_register + op.as.cast.target)


#define binary_op_suite(opnampe, op)\
case bc_##opnampe##_u64:\
	binary_op_case(op, u64);\
	break;\
case bc_##opnampe##_i64:\
	binary_op_case(op, i64);\
	break;\
case bc_##opnampe##_u32:\
	binary_op_case(op, u32);\
	break;\
case bc_##opnampe##_i32:\
	binary_op_case(op, i32);\
	break;\
case bc_##opnampe##_u16:\
	binary_op_case(op, u16);\
	break;\
case bc_##opnampe##_i16:\
	binary_op_case(op, i16);\
	break;\
case bc_##opnampe##_u8:\
	binary_op_case(op, u8);\
	break;\
case bc_##opnampe##_i8:\
	binary_op_case(op, i8);\
	break;\
case bc_##opnampe##_f64:\
	binary_op_case(op, f64);\
	break;\
case bc_##opnampe##_f32:\
	binary_op_case(op, f32);\
	break;\
case bc_##opnampe##_bool:\
	binary_op_case(op, bool);\
	break;\

#define cast_op_suite(to_type)\
case bc_cast_u64_##to_type:\
	cast_case(to_type, u64);\
	break;\
case bc_cast_i64_##to_type:\
	cast_case(to_type, i64);\
	break;\
case bc_cast_u32_##to_type:\
	cast_case(to_type, u32);\
	break;\
case bc_cast_i32_##to_type:\
	cast_case(to_type, i32);\
	break;\
case bc_cast_u16_##to_type:\
	cast_case(to_type, u16);\
	break;\
case bc_cast_i16_##to_type:\
	cast_case(to_type, i16);\
	break;\
case bc_cast_u8_##to_type:\
	cast_case(to_type, u8);\
	break;\
case bc_cast_i8_##to_type:\
	cast_case(to_type, i8);\
	break;\
case bc_cast_f64_##to_type:\
	cast_case(to_type, f64);\
	break;\
case bc_cast_f32_##to_type:\
	cast_case(to_type, f32);\
	break;\
case bc_cast_bool_##to_type:\
	cast_case(to_type, bool);\
	break;


register_id vm_run(vm* vm, bytecode_context* bcc)
{
	call_frame* current_frame = &vm->frames[vm->fc - 1];

	while(true)
	{
		bytecode_op op = bcc->bc.data[vm->ip];
        switch (op.instr)
        {
			case bc_get_sp:
				vm->registers[current_frame->base_register + op.as.get_sp.to] = vm->stack_pointer;
				break;

			case bc_push:
				vm->stack_pointer += op.as.push.offset;
				break;
					
			case bc_store:
				memcpy(&vm->stack[vm->registers[current_frame->base_register + op.as.store.to_addr] + op.as.store.offset], &vm->registers[current_frame->base_register + op.as.store.from], op.as.store.size);
				break;

			case bc_jmp:
				vm->ip += op.as.jmp.offset;
				continue; // skip ip increment
				break;
			case bc_jmp_if:
				if (vm->registers[current_frame->base_register + op.as.jmp_if.cond_reg])
				{
					vm->ip += op.as.jmp_if.offset;
					continue; // skip ip increment
				}
				break;
			case bc_jmp_if_not:		
				if (!vm->registers[current_frame->base_register + op.as.jmp_if_not.cond_reg])
				{
					vm->ip += op.as.jmp_if_not.offset;
					continue; // skip ip increment
				}
				break;
            case bc_mov_imm:
				
				memcpy(vm->registers + current_frame->base_register + op.as.mov_imm.to, &op.as.mov_imm.value, op.as.mov_imm.size);
                break;
			case bc_mov:
				memcpy(vm->registers + current_frame->base_register + op.as.mov.to, vm->registers + current_frame->base_register + op.as.mov.from, sizeof(value64));
                break;

            binary_op_suite(add, +);
            binary_op_suite(sub, -);
            binary_op_suite(div, /);
            binary_op_suite(mul, *);
            binary_op_suite(cmp_lt, <);
            binary_op_suite(cmp_lte, <=);
            binary_op_suite(cmp_gt, >);
            binary_op_suite(cmp_gte, >=);
            binary_op_suite(cmp_eq, ==);
            binary_op_suite(cmp_neq, !=);

			cast_op_suite(i8);
			cast_op_suite(u8);

			cast_op_suite(i16);
			cast_op_suite(u16);

			cast_op_suite(i32);
			cast_op_suite(u32);

			cast_op_suite(i64);
			cast_op_suite(u64);

			cast_op_suite(f32);
			cast_op_suite(f64);

			case bc_cast_u64_bool:
				cast_case(bool, u64);
				break;
			case bc_cast_i64_bool:
				cast_case(bool, i64);
				break;
			case bc_cast_u32_bool:
				cast_case(bool, u32);
				break;
			case bc_cast_i32_bool:
				cast_case(bool, i32);
				break;
			case bc_cast_u16_bool:
				cast_case(bool, u16);
				break;
			case bc_cast_i16_bool:
				cast_case(bool, i16);
				break;
			case bc_cast_u8_bool:
				cast_case(bool, u8);
				break;
			case bc_cast_i8_bool:
				cast_case(bool, i8);
				break;
			case bc_cast_f64_bool:
				cast_case(bool, f64);
				break;
			case bc_cast_f32_bool:
				cast_case(bool, f32);
				break;

			case bc_call:
			{
				if(op.as.call.intrinsic)
				{
					printf((char*)(vm->stack + *(u64*)&vm->registers[current_frame->base_register + op.as.call.first_arg]), *(u64*)&vm->registers[current_frame->base_register + op.as.call.first_arg + 1]);
					vm->ip++;
					continue;
				}

			 	bytecode_fn* target_fn = bcc->fns.data + op.as.call.function_index;

                // current_frame->ip++;

                call_frame new_frame = {0};
                new_frame.fn = target_fn;

                new_frame.base_register = current_frame->base_register + current_frame->fn->reg_counter;
                new_frame.ret_reg = op.as.call.dest;
				new_frame.ret_ip = vm->ip + 1;

				for(uz i = 0; i < op.as.call.arg_count; i++)
				{
					memcpy(&vm->registers[new_frame.base_register + i], &vm->registers[current_frame->base_register + op.as.call.first_arg + i], sizeof(value64));
				}

                vm->frames[vm->fc++] = new_frame;
                current_frame = &vm->frames[vm->fc - 1];
				vm->ip = target_fn->entry_ip;

                continue;	
			}
            case bc_ret:
			{
				u64 return_ip = current_frame->ret_ip;				
				register_id ret_reg = current_frame->base_register + op.as.ret.reg;
				register_id target_reg = current_frame->ret_reg;
				
				vm->fc--;
				
				if(vm->fc == 0) { return ret_reg; }
				
				current_frame = &vm->frames[vm->fc - 1];
				
				
				if(!op.as.ret.is_void)
				{
					memcpy(vm->registers + current_frame->base_register + target_reg, vm->registers + ret_reg, sizeof(value64));
				}

				vm->ip = return_ip;

				continue;
			}

            default:
                printf("unknown instruction at ip: %llu\n", vm->ip);
                return null_register;
        }

		vm->ip++;

	}
}


i64* run_bytecode(vm* vm, bytecode_context* bcc)
{
	call_frame* frame = vm->frames + vm->fc++;
	frame->fn = bcc->fns.data + bytecode_find_function_id(bcc, "main");
	frame->base_register = 0;
	frame->ret_reg = 0;
	vm->ip = frame->fn->entry_ip;

	register_id ret_reg = vm_run(vm, bcc);

	if(ret_reg != null_register)
		return (i64*)&vm->registers[ret_reg];

	return NULL;
}
