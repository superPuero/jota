#ifndef jota_vm
#define jota_vm

#include "bytecode.h"
#include "symbol.h"

typedef struct 
{
	bytecode_fn* fn;
	register_id base_register;
	register_id ret_reg;
	u64 ret_ip;
} call_frame;

typedef struct
{
	u8  stack[Mb(1)];
	u64 stack_pointer;
	value64 registers[65565];
	u64 ip;
	call_frame frames[64];
	u64 fc;	
} vm;

i64* run_bytecode(vm* vm, bytecode_context* bcc);

#endif