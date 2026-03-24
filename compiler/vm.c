#include "vm.h"

#define jo_binary_op_case(operation, type)\
*(type*)(vm->registers + call_frame->base_register + op.as.binary_op.dest) = \
                    *(type*)(vm->registers + call_frame->base_register + op.as.binary_op.a) operation \
                    *(type*)(vm->registers + call_frame->base_register + op.as.binary_op.b)

#define jo_cast_case(to_type, from_type)\
*(jo_##to_type*)(vm->registers + call_frame->base_register + op.as.cast.dest) = \
                   (jo_##to_type)*(from_type*)(vm->registers + call_frame->base_register + op.as.cast.target)


#define jo_binary_op_suite(opnampe, op)\
case jo_bytecode_instr_##opnampe##_u64:\
	jo_binary_op_case(op, jo_u64);\
	break;\
case jo_bytecode_instr_##opnampe##_i64:\
	jo_binary_op_case(op, jo_i64);\
	break;\
case jo_bytecode_instr_##opnampe##_u32:\
	jo_binary_op_case(op, jo_u32);\
	break;\
case jo_bytecode_instr_##opnampe##_i32:\
	jo_binary_op_case(op, jo_i32);\
	break;\
case jo_bytecode_instr_##opnampe##_u16:\
	jo_binary_op_case(op, jo_u16);\
	break;\
case jo_bytecode_instr_##opnampe##_i16:\
	jo_binary_op_case(op, jo_i16);\
	break;\
case jo_bytecode_instr_##opnampe##_u8:\
	jo_binary_op_case(op, jo_u8);\
	break;\
case jo_bytecode_instr_##opnampe##_i8:\
	jo_binary_op_case(op, jo_i8);\
	break;\
case jo_bytecode_instr_##opnampe##_f64:\
	jo_binary_op_case(op, jo_f64);\
	break;\
case jo_bytecode_instr_##opnampe##_f32:\
	jo_binary_op_case(op, jo_f32);\
	break;\
case jo_bytecode_instr_##opnampe##_bool:\
	jo_binary_op_case(op, bool);\
	break;\

#define jo_cast_op_suite(to_type)\
case jo_bytecode_instr_cast_u64_##to_type:\
	jo_cast_case(to_type, jo_u64);\
	break;\
case jo_bytecode_instr_cast_i64_##to_type:\
	jo_cast_case(to_type, jo_i64);\
	break;\
case jo_bytecode_instr_cast_u32_##to_type:\
	jo_cast_case(to_type, jo_u32);\
	break;\
case jo_bytecode_instr_cast_i32_##to_type:\
	jo_cast_case(to_type, jo_i32);\
	break;\
case jo_bytecode_instr_cast_u16_##to_type:\
	jo_cast_case(to_type, jo_u16);\
	break;\
case jo_bytecode_instr_cast_i16_##to_type:\
	jo_cast_case(to_type, jo_i16);\
	break;\
case jo_bytecode_instr_cast_u8_##to_type:\
	jo_cast_case(to_type, jo_u8);\
	break;\
case jo_bytecode_instr_cast_i8_##to_type:\
	jo_cast_case(to_type, jo_i8);\
	break;\
case jo_bytecode_instr_cast_f64_##to_type:\
	jo_cast_case(to_type, jo_f64);\
	break;\
case jo_bytecode_instr_cast_f32_##to_type:\
	jo_cast_case(to_type, jo_f32);\
	break;\
case jo_bytecode_instr_cast_bool_##to_type:\
	jo_cast_case(to_type, bool);\
	break;


jo_register_id jo_vm_run(jo_vm* vm, jo_bytecode_context* bcc)
{
	jo_call_frame* call_frame = &vm->frames[vm->fc - 1];

	while(true)
	{
		jo_bytecode_fn* fn = call_frame->fn;
		jo_bytecode_op op = bcc->bc.data[vm->ip];

        switch (op.instr)
        {
			case jo_bytecode_instr_jmp:
				vm->ip += op.as.jmp.offset;
				continue; // skip ip increment
				break;
			case jo_bytecode_instr_jmp_if:
				if (vm->registers[op.as.jmp_if.cond_reg])
				{
					vm->ip += op.as.jmp_if.offset;
					continue; // skip ip increment
				}
				break;
			case jo_bytecode_instr_jmp_if_not:
				if (!vm->registers[op.as.jmp_if_not.cond_reg])
				{
					vm->ip += op.as.jmp_if_not.offset;
					continue; // skip ip increment
				}
				break;
            case jo_bytecode_instr_mov_imm:
				memcpy(vm->registers + call_frame->base_register + op.as.mov_imm.to, &op.as.mov_imm.value, sizeof(jo_value64));
                break;

			case jo_bytecode_instr_mov:
				memcpy(vm->registers + call_frame->base_register + op.as.mov.to, vm->registers + call_frame->base_register + op.as.mov.from, sizeof(jo_value64));
                break;

            jo_binary_op_suite(add, +);
            jo_binary_op_suite(sub, -);
            jo_binary_op_suite(div, /);
            jo_binary_op_suite(mul, *);
            jo_binary_op_suite(cmp_lt, <);
            jo_binary_op_suite(cmp_lte, <=);
            jo_binary_op_suite(cmp_gt, >);
            jo_binary_op_suite(cmp_gte, >=);
            jo_binary_op_suite(cmp_eq, ==);
            jo_binary_op_suite(cmp_neq, !=);

			jo_cast_op_suite(i8);
			jo_cast_op_suite(u8);

			jo_cast_op_suite(i16);
			jo_cast_op_suite(u16);

			jo_cast_op_suite(i32);
			jo_cast_op_suite(u32);

			jo_cast_op_suite(i64);
			jo_cast_op_suite(u64);

			jo_cast_op_suite(f32);
			jo_cast_op_suite(f64);

			case jo_bytecode_instr_cast_u64_bool:
				jo_cast_case(bool, jo_u64);
				break;
			case jo_bytecode_instr_cast_i64_bool:
				jo_cast_case(bool, jo_i64);
				break;
			case jo_bytecode_instr_cast_u32_bool:
				jo_cast_case(bool, jo_u32);
				break;
			case jo_bytecode_instr_cast_i32_bool:
				jo_cast_case(bool, jo_i32);
				break;
			case jo_bytecode_instr_cast_u16_bool:
				jo_cast_case(bool, jo_u16);
				break;
			case jo_bytecode_instr_cast_i16_bool:
				jo_cast_case(bool, jo_i16);
				break;
			case jo_bytecode_instr_cast_u8_bool:
				jo_cast_case(bool, jo_u8);
				break;
			case jo_bytecode_instr_cast_i8_bool:
				jo_cast_case(bool, jo_i8);
				break;
			case jo_bytecode_instr_cast_f64_bool:
				jo_cast_case(bool, jo_f64);
				break;
			case jo_bytecode_instr_cast_f32_bool:
				jo_cast_case(bool, jo_f32);
				break;

			case jo_bytecode_instr_call:
			{
			 	jo_bytecode_fn* target_fn = bcc->fns.data + op.as.call.function_index;

                // call_frame->ip++;



                jo_call_frame new_frame = {0};
                new_frame.fn = target_fn;

                new_frame.base_register = call_frame->base_register + call_frame->fn->reg_counter;
                new_frame.ret_reg = op.as.call.dest;
				new_frame.ret_ip = vm->ip + 1;

				for(jo_usize i = 0; i < op.as.call.arg_count; i++)
				{
					memcpy(&vm->registers[new_frame.base_register + i], &vm->registers[call_frame->base_register + op.as.call.first_arg + i], sizeof(jo_value64));
				}

                vm->frames[vm->fc++] = new_frame;
                call_frame = &vm->frames[vm->fc - 1];
				vm->ip = target_fn->entry_ip;

                continue;
			}
            case jo_bytecode_instr_ret:
			{
				jo_u32 return_ip = call_frame->ret_ip;				
				jo_register_id ret_reg = call_frame->base_register + op.as.ret.reg;
				jo_register_id target_reg = call_frame->ret_reg;
				
				vm->fc--;
				
				if(vm->fc == 0) { return ret_reg; }
				
				call_frame = &vm->frames[vm->fc - 1];
				
				
				if(!op.as.ret.is_void)
				{
					memcpy(vm->registers + call_frame->base_register + target_reg, vm->registers + ret_reg, sizeof(jo_value64));
				}

				vm->ip = return_ip;

				continue;
			}

            default:
                printf("unknown instruction at ip: %d\n", vm->ip);
                return jo_null_register;
        }

		// call_frame->ip++;
		vm->ip++;

	}
}


void jo_run_bytecode(jo_bytecode_context* bcc)
{
	jo_vm vm = {0};

	jo_call_frame* frame = vm.frames + vm.fc++;
	frame->fn = bcc->fns.data + jo_bytecode_find_function(bcc, "main");
	frame->base_register = 0;
	frame->ret_reg = 0;
	vm.ip = bcc->fns.data[jo_bytecode_find_function(bcc, "main")].entry_ip;

	jo_register_id ret_reg = jo_vm_run(&vm, bcc);

	if(ret_reg != jo_null_register)
		printf("program returned: %d\n", *(jo_i64*)&vm.registers[ret_reg]);
}
