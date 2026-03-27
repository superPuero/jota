#ifndef jota_vm
#define jota_vm

#include "bytecode.h"
#include "symbol.h"

typedef struct 
{
	jo_bytecode_fn* fn;
	jo_register_id base_register;
	jo_register_id ret_reg;
	jo_u64 ret_ip;
} jo_call_frame;

typedef struct
{
	jo_u8  stack[1024 * 1024]; // ~1 MB of stack memory	
	jo_u8* stack_pointer;

	jo_value64 registers[65565];

	jo_u64 ip;

	jo_call_frame frames[64];
	jo_u64 fc;	
} jo_vm;

jo_i64* jo_run_bytecode(jo_vm* vm, jo_bytecode_context* bcc);

#endif