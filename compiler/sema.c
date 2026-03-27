#include "sema.h"
#include "ast_node.h"
#include "parser.h"
#include "symbol.h"

#include <assert.h>
#include <stdio.h>

#define jo_sema_resolve_primitive_type_case(type)\
case jo_ast_type_literal_##type:\
	{\
		jo_ast_node_t* type_node = jo_ast_node_make(sema->arena, jo_ast_type_type_primitive);\
		type_node->data.type_primitive = jo_token_keyword_##type;\
		node->resolved_type = type_node;\
		break;\
	}

bool jo_type_is_pointer(jo_ast_node_t* type)
{
	return type->type == jo_ast_type_type_ptr;
}

jo_u32 jo_get_primitive_type_size(jo_token_type_t type)
{
	jo_u32 size = 0;
	switch (type)
	{
	case jo_token_keyword_i64:
	case jo_token_keyword_u64:
	case jo_token_keyword_f64:
		size = 8;
		break;

	case jo_token_keyword_i32:
	case jo_token_keyword_u32:
	case jo_token_keyword_f32:
		size = 4;
		break;

	case jo_token_keyword_i16:
	case jo_token_keyword_u16:
		size = 2;
		break;

	case jo_token_keyword_i8:
	case jo_token_keyword_u8:
	case jo_token_keyword_bool:
		size = 1;
		break;

	case jo_token_keyword_void:
		size = 0;
		break;
	
	default:
		assert(0);
		break;
	}

	return size;
}

bool jo_primitive_type_is_fp(jo_token_type_t type)
{
	return type == jo_token_keyword_f32 || type == jo_token_keyword_f64;
}


bool jo_primitive_type_is_integer(jo_token_type_t type)
{
	switch (type)
	{
	case jo_token_keyword_i64:
		// fall-through
	case jo_token_keyword_i32:
		// fall-through
	case jo_token_keyword_i16:
		// fall-through
	case jo_token_keyword_i8:
		// fall-through
	case jo_token_keyword_u64:
		// fall-through
	case jo_token_keyword_u32:
		// fall-through
	case jo_token_keyword_u16:
		// fall-through
	case jo_token_keyword_u8:
		return true;
	default:
		return false;
	}
}


bool jo_is_integer_primitive_type_signed(jo_token_type_t type)
{
	bool sign = false;
	switch (type)
	{
	case jo_token_keyword_i64:
	case jo_token_keyword_i32:
	case jo_token_keyword_i16:
	case jo_token_keyword_i8:
		sign = true;
		break;
		
	case jo_token_keyword_u64:
	case jo_token_keyword_u32:
	case jo_token_keyword_u16:
	case jo_token_keyword_u8:
		sign = false;
		break;

	default:
		assert(0);
		break;
	}

	return sign;
}

bool jo_is_primitive_type_implicitly_castable_to(jo_token_type_t primitive_from, jo_token_type_t primitive_to)
{
	if(primitive_from == jo_token_keyword_bool) { return true; }
	if(primitive_to == jo_token_keyword_bool) { return true; }

	if(jo_primitive_type_is_fp(primitive_from) != jo_primitive_type_is_fp(primitive_to)) { return false; }

	if(jo_get_primitive_type_size(primitive_from) < jo_get_primitive_type_size(primitive_to)) { return false; }

	if(!jo_primitive_type_is_fp(primitive_from) && !jo_primitive_type_is_fp(primitive_to))
	{
		return jo_is_integer_primitive_type_signed(primitive_from) == jo_is_integer_primitive_type_signed(primitive_to);
	}

	return false;
}

bool jo_type_is_primitive(jo_ast_node_t* type)
{
	return type->type == jo_ast_type_type_primitive;
}


jo_astr_t jo_sema_type_astr(jo_arena_t* arena, jo_ast_node_t* t1)
{
	if(!t1) assert(0);

	switch (t1->type)
	{
	case jo_ast_type_type_primitive:
		return jo_astr_from(arena, jo_token_type_to_string(t1->data.type_primitive + 17)); //@TODO: make it more understandalbe, 17 affset is to remove jo_token_keyword_* prefix 
		break;

	case jo_ast_type_type_fn:
		jo_ast_type_fn* type_fn = &t1->data.type_fn;
		jo_astr_t str = jo_astr_from(arena, "fn(");

		for(jo_uz param_i = 0; param_i < t1->data.type_fn.parameters.occupied; param_i++)
		{
			jo_astr_t param_str = jo_sema_type_astr(arena, t1->data.type_fn.parameters.data[param_i]);
	
			jo_astr_append_astr(arena, &str, &param_str);
	
			if(param_i != t1->data.type_fn.parameters.occupied - 1)
			{
				jo_astr_append(arena, &str, ", ");			
			}
		}


		jo_astr_append(arena, &str, ")");

		
		if(type_fn->return_type)
		{
			jo_astr_append(arena, &str, " -> ");

			jo_astr_t ret_str = jo_sema_type_astr(arena, t1->data.type_fn.return_type);
			jo_astr_append(arena, &str, ret_str.data);
		}

		return str;
		break;
	

	default:
		assert(0);
		break;
	}
}

bool jo_sema_type_is_void(jo_ast_node_t* type_node)
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
		return type_node->data.type_primitive == jo_token_keyword_void;
	}

}

bool jo_sema_types_are_equal(jo_ast_node_t* t1, jo_ast_node_t* t2)
{
	if(!t1)
	{
		assert(0 && "unresolved t1\n");
		return false;
	}

	if(!t2)
	{
		assert(0 && "unresolved t2\n");
		return false;
	}

	if(t2->type != t1->type) return false;

	if(t1->type == jo_ast_type_type_primitive)
	{
		return t1->data.type_primitive == t2->data.type_primitive;
	}
	else if(t1->type == jo_ast_type_type_fn)
	{
		jo_ast_type_fn* fn1 = &t1->data.type_fn;
		jo_ast_type_fn* fn2 = &t2->data.type_fn;

		if(fn1->parameters.occupied != fn2->parameters.occupied)
		{
			return false;
		}

		bool pass = true;

		for(jo_uz arg_i = 0; arg_i < fn1->parameters.occupied; arg_i++)
		{
			pass = jo_sema_types_are_equal(fn1->parameters.data[arg_i]->data.decl.specified_type, fn2->parameters.data[arg_i]->data.decl.specified_type);
			if(!pass) return false;
		}

		return jo_sema_types_are_equal(fn1->return_type, fn2->return_type);
	}
	else if(t1->type == jo_ast_type_type_ptr)
	{
		return jo_sema_types_are_equal(t1->data.type_ptr.inner, t2->data.type_ptr.inner);
	}
	else if(t1->type == jo_ast_type_type_ref)
	{
		return jo_sema_types_are_equal(t1->data.type_ref.inner, t2->data.type_ref.inner);
	}
	else if(t1->type == jo_ast_type_type_array)
	{
		if(t1->data.type_array.array_size_expression && t2->data.type_array.array_size_expression)
		{
			
		}
		return false;
	}
	else if(t1->type == jo_ast_type_type_type)
	{
		return jo_sema_types_are_equal(t1->data.type_type.type_node, t2->data.type_type.type_node);
	}

	return false;
}

void jo_sema_diagnose_type_implic_castabability_to(jo_ast_node_t* t1, jo_ast_node_t* t2)
{
	if(jo_sema_types_are_equal(t1, t2)) return;


}

void jo_sema_resolve_expr_op_call(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* expr)
{
	if(expr->resolved_type) return;

	jo_symbol_t* sym = jo_scope_lookup_symbol(outer_scope, expr->data.expr_op_call.target->data.identifier);
	
	if(!sym)
	{
	    printf("call of undeclared identifier %.*s\n", expr->data.expr_op_call.target->data.identifier.len, expr->data.expr_op_call.target->data.identifier.data);
		assert(0);
	}

	if(sym->ast_node->resolved_type->data.type_fn.parameters.occupied != expr->data.expr_op_call.arguments.occupied)
	{
		printf("argument coutnt missmatch, expected %llu, got %llu\n", 
			sym->ast_node->resolved_type->data.type_fn.parameters.occupied, 
			expr->data.expr_op_call.arguments.occupied);
		assert(0);
	}

	for(jo_uz arg_i = 0; arg_i < expr->data.expr_op_call.arguments.occupied; arg_i++)
	{
		jo_sema_resolve_expr(sema, outer_scope, expr->data.expr_op_call.arguments.data[arg_i]);
		if(!jo_sema_types_are_equal(
			sym->ast_node->resolved_type->data.type_fn.parameters.data[arg_i], 
			expr->data.expr_op_call.arguments.data[arg_i]->resolved_type))
		{
			jo_arena_scope(sema->arena)
			{
				jo_astr_t expr_str = jo_sema_type_astr(sema->arena, sym->ast_node->resolved_type->data.type_fn.parameters.data[arg_i]);
				jo_astr_t resolved_str = jo_sema_type_astr(sema->arena, expr->data.expr_op_call.arguments.data[arg_i]->resolved_type);
				printf("argument provided for %.*s call at index %llu has unexpected type, expected %.*s got %.*s",
					jo_astr_fmt(&sym->identifier),
					arg_i,
					jo_astr_fmt(&expr_str),
					jo_astr_fmt(&resolved_str)
				);
			}

			assert(0);
		}
	}

	expr->resolved_type = sym->ast_node->resolved_type->data.type_fn.return_type;
}

void jo_sema_resolve_expr_op_binary_type(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* expr)
{
	if(expr->resolved_type) return;

	jo_sema_resolve_expr(sema, outer_scope, expr->data.expr_op_binary.left_expression);
	jo_sema_resolve_expr(sema, outer_scope, expr->data.expr_op_binary.right_expression);

	jo_ast_node_t* left_expr_type = expr->data.expr_op_binary.left_expression->resolved_type;
	jo_ast_node_t* right_expr_type = expr->data.expr_op_binary.right_expression->resolved_type;

	if(jo_sema_types_are_equal(left_expr_type, right_expr_type))
	{
		expr->resolved_type = left_expr_type;
	}
	else
	{ 
		if(!jo_sema_types_are_equal(left_expr_type, right_expr_type))
		{
			// jo_string spec_type_str = jo_sema_type_string(left_expr_type);
			// jo_string resolved_type_str = jo_sema_type_string(right_expr_type);
			
			// printf("%s is not implicitly castable to %s\n", spec_type_str.data, resolved_type_str.data);
			
			assert(0);
		}
	}
}

void jo_sema_resolve_expr_op_unary_type(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* expr)
{
	if(expr->resolved_type) return;

	jo_sema_resolve_expr(sema, outer_scope, expr->data.expr_op_unary.expression);

	jo_ast_node_t* left_expr_type = expr->data.expr_op_unary.expression->resolved_type;

	if(jo_type_is_primitive(left_expr_type))
	{
		if(jo_primitive_type_is_integer(left_expr_type->data.type_primitive))
		{
			
		}
	}
}


void jo_sema_resolve_expr_assigment(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* node)
{
	jo_sema_resolve_expr(sema, outer_scope, node->data.expr_assignment.expression);

	jo_symbol_t* sym = jo_scope_lookup_symbol(outer_scope, node->data.expr_assignment.target->data.identifier);

	if(!sym)
	{
		printf("undeclared identifier %.*s", jo_str_view_fmt(&node->data.expr_assignment.target->data.identifier));
		assert(0);
	}


	if(!jo_sema_types_are_equal(sym->ast_node->resolved_type, node->data.expr_assignment.expression->resolved_type))
	{
		jo_arena_scope(sema->arena)
		{
			jo_astr_t gtsrt = jo_sema_type_astr(sema->arena, sym->ast_node->resolved_type);
			jo_astr_t rtstr = jo_sema_type_astr(sema->arena, node->data.expr_assignment.expression->resolved_type);
			printf("can not assign expression of a type %.*s to variable %.*s of type %.*s"
					, jo_astr_fmt(&rtstr)
					, jo_str_view_fmt(&node->data.expr_assignment.target->data.identifier)
					, jo_astr_fmt(&gtsrt)
				);
		}

		assert(0);
	}

}

void jo_sema_resolve_expr_as_cast(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* node)
{
	jo_sema_resolve_expr(sema, outer_scope, node->data.expr_as_cast.expr);
	node->resolved_type = node->data.expr_as_cast.to_type;	
}

void jo_sema_resolve_expr(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* node)
{
	if(node->resolved_type) return;

	switch (node->type)
	{
		jo_sema_resolve_primitive_type_case(i64);
		jo_sema_resolve_primitive_type_case(u64);
		jo_sema_resolve_primitive_type_case(f64);

	case jo_ast_type_literal_string:
	{
		jo_ast_node_t* array_type_node = jo_ast_node_make(sema->arena, jo_ast_type_type_array);
		array_type_node->data.type_array.array_size_expression = jo_ast_node_make(sema->arena, jo_ast_type_literal_u64);
		array_type_node->data.type_array.array_size_expression->data.literal_u64 = node->data.literal_string.len;
		array_type_node->data.type_array.inner = jo_ast_node_make(sema->arena,jo_ast_type_type_primitive);
		array_type_node->data.type_array.inner->data.type_primitive = jo_token_keyword_u8;
		node->resolved_type = array_type_node;
		break;
	}

	case jo_ast_type_literal_fn:
    {
        jo_ast_node_t* type_node = jo_ast_node_make(sema->arena,jo_ast_type_type_fn);
        type_node->data.type_fn.return_type = node->data.literal_fn.return_type;

		jo_ada_foreach(&node->data.literal_fn.parameters)
		{
			jo_ada_append(sema->arena, &type_node->data.type_fn.parameters, (*node->data.literal_fn.parameters.it)->data.decl.specified_type);
		}

        node->resolved_type = type_node;
   	    break;
    }

	case jo_ast_type_literal_struct:
    {
        jo_ast_node_t* type_node = jo_ast_node_make(sema->arena,jo_ast_type_type_struct);

		jo_ada_foreach(&node->data.literal_struct.members)
		{
			jo_sema_resolve_expr(sema, outer_scope, *node->data.literal_struct.members.it);
			jo_ada_append(sema->arena, &type_node->data.type_struct.members, (*node->data.literal_struct.members.it)->resolved_type);
		}

        node->resolved_type = type_node;
   	    break;
    }

	case jo_ast_type_literal_bool:
	{
		jo_ast_node_t* type_node = jo_ast_node_make(sema->arena,jo_ast_type_type_primitive);
		type_node->data.type_primitive = jo_token_keyword_bool;
		node->resolved_type = type_node;
		break;
	}

	case jo_ast_type_decl:
	{
        jo_sema_resolve_decl(sema, outer_scope, node);

		jo_symbol_t sym = {0};
		sym.ast_node = node;
		sym.identifier = jo_astr_from(sema->arena, node->data.decl.identifier->data.identifier.data);		
		jo_scope_add_symbol(sema->arena,outer_scope, sym);		

		//@TODO: maybe there is a better way 
		jo_sema_resolve_expr(sema, outer_scope, node->data.decl.identifier);		
		break;
	}
	case jo_ast_type_identifier:
	{
		jo_symbol_t* sym = jo_scope_lookup_symbol(outer_scope, node->data.identifier);
		if(!sym)
		{
			printf("undeclared identifier %.*s", jo_str_view_fmt(&node->data.identifier));
			assert(0);
		}
		
		node->resolved_type = sym->ast_node->resolved_type;
		node->resolved_symbol = sym;			
		break;
	}

	case jo_ast_type_expr_as_cast:
		jo_sema_resolve_expr_as_cast(sema, outer_scope, node);
		break;
	case jo_ast_type_expr_op_binary:
		jo_sema_resolve_expr_op_binary_type(sema, outer_scope, node);
		break;
	case jo_ast_type_expr_op_unary:
		jo_sema_resolve_expr_op_unary_type(sema, outer_scope, node);
		break;
	case jo_ast_type_expr_op_call:
		jo_sema_resolve_expr_op_call(sema, outer_scope, node);
		break;
	default:
		assert(0);
		break;
	}
}

void jo_sema_resolve_stmt(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* stmt, jo_ast_node_t* literal_parent_fn_node)
{
	jo_ast_literal_fn* literal_parent_fn = &literal_parent_fn_node->data.literal_fn;

	switch (stmt->type)
	{
	case jo_ast_type_stmt_expr:
		jo_sema_resolve_expr(sema, outer_scope, stmt->data.stmt_expr.expr);

		break;
	case jo_ast_type_stmt_ifelse:
		// jo_sema_insert_as_cast(sema, outer_scope, stmt->data.stmt_ifelse.condition, jo_token_keyword_bool);
		jo_sema_resolve_expr(sema, outer_scope, stmt->data.stmt_ifelse.condition);
		jo_sema_resolve_block(sema, outer_scope, stmt->data.stmt_ifelse.true_block, literal_parent_fn_node);

		if(stmt->data.stmt_ifelse.tail_stmt_if)
		{
			jo_sema_resolve_stmt(sema, outer_scope, stmt->data.stmt_ifelse.tail_stmt_if, literal_parent_fn_node);
		}
		else if(stmt->data.stmt_ifelse.false_block)
		{
			jo_sema_resolve_block(sema, outer_scope, stmt->data.stmt_ifelse.false_block, literal_parent_fn_node);
		}
		break;

	case jo_ast_type_stmt_return:
		if(!literal_parent_fn_node)
		{
			assert(0 && "non fn block cant have return statement");
		}
	
		if(stmt->data.stmt_return.expression)
		{
			jo_sema_resolve_expr(sema, outer_scope, stmt->data.stmt_return.expression);

			if(!jo_sema_types_are_equal(stmt->data.stmt_return.expression->resolved_type, literal_parent_fn->return_type))
			{
				jo_arena_scope(sema->arena)
				{
					jo_astr_t estr = jo_sema_type_astr(sema->arena, literal_parent_fn->return_type);
					jo_astr_t gstr = jo_sema_type_astr(sema->arena, stmt->data.stmt_return.expression->resolved_type);

					printf("function return type %.*s does not match return expression type %.*s"
						, jo_astr_fmt(&estr)
						, jo_astr_fmt(&gstr)
					);
				}
				assert(0);
			}
		}
		else
		{
			if(!jo_sema_type_is_void(literal_parent_fn->return_type))
			{
				jo_arena_scope(sema->arena)
				{
					jo_astr_t estr = jo_sema_type_astr(sema->arena, literal_parent_fn->return_type);
					jo_astr_t gstr = jo_sema_type_astr(sema->arena, stmt->data.stmt_return.expression->resolved_type);

					printf("function return type %.*s does not match return expression type %.*s"
						, jo_astr_fmt(&estr)
						, jo_astr_fmt(&gstr)
					);
				}
				assert(0);
			}
		}



		break;
	case jo_ast_type_block:
		jo_sema_resolve_block(sema, outer_scope, stmt, literal_parent_fn_node);

		break;
	default:
		assert(0);
		break;
	}
}

void jo_sema_resolve_block(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* block, jo_ast_node_t* literal_parent_fn_node)
{
	jo_scope_t* fn_block_scope = jo_scope_push(sema->arena, outer_scope, jo_str_view_from_cstr("block"));
	
	for(jo_uz i = 0; i < block->data.block.statements.occupied; i++)
	{
		jo_ast_node_t* stmt = block->data.block.statements.data[i];
		
		jo_sema_resolve_stmt(sema, fn_block_scope, stmt, literal_parent_fn_node);
	}
}

void jo_sema_analyze_literal_fn(jo_sema_t* sema, jo_scope_t* fn_scope, jo_ast_node_t* literal_fn_node)
{	
	jo_ast_literal_fn* literal_fn = &literal_fn_node->data.literal_fn;
	
	if(literal_fn->intrinsic) return;

    jo_ada_foreach(&literal_fn->parameters)
	{			
		jo_ast_node_t* param_node = *literal_fn->parameters.it;
		jo_sema_resolve_decl(sema, fn_scope, param_node);

		jo_symbol_t sym = {0};
		sym.identifier = jo_astr_from_view(sema->arena, param_node->data.decl.identifier->data.identifier);

		sym.kind = jo_symbol_kind_variable;
		sym.ast_node = param_node;
		param_node->resolved_symbol = jo_scope_add_symbol(sema->arena,fn_scope, sym);
	}

	jo_sema_resolve_block(sema, fn_scope, literal_fn->block, literal_fn_node);
}

void jo_analyze_decl(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* decl_node)
{
	assert(decl_node->resolved_type);

	switch (decl_node->resolved_type->type)
	{
	case jo_ast_type_type_fn:
	{
		jo_scope_t* fn_scope = jo_scope_push(sema->arena, outer_scope, decl_node->data.decl.identifier->data.identifier);
		jo_sema_analyze_literal_fn(sema, fn_scope, decl_node->data.decl.initialize_expression);
		break;
	}

	case jo_ast_type_type_struct:
		break;

	
	default:
		break;
	}
}

void jo_sema_analyze_module(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* module_node)
{
    jo_ast_module* module = &module_node->data.module;

    // module-space declaration are position independent
	for(jo_uz i = 0; i < module->content.occupied; i++)
	{
	    jo_ast_node_t* current_content = module->content.data[i];

		switch (module->content.data[i]->type)
		{
			case jo_ast_type_decl:
			{
                jo_sema_resolve_decl(sema, outer_scope, current_content);

				jo_symbol_t sym = {0};
				sym.kind = jo_symbol_kind_function;
				sym.ast_node = current_content;
				sym.identifier = jo_astr_from_view(sema->arena ,current_content->data.decl.identifier->data.identifier);				
				current_content->resolved_symbol = jo_scope_add_symbol(sema->arena,outer_scope, sym);
				break;
			}

		default:
			break;
		}
	}
  
	for(jo_uz i = 0; i < module->content.occupied; i++)	
	{
	    jo_ast_node_t* current_content = module->content.data[i];

		switch (current_content->type)
		{
		case jo_ast_type_decl:
			jo_analyze_decl(sema, &sema->global_scope, current_content);
			break;

		default:
			break;
		}
	}

}



void jo_sema_resolve_decl(jo_sema_t* sema, jo_scope_t* outer_scope, jo_ast_node_t* decl_node)
{
    jo_ast_decl* decl = &decl_node->data.decl;

	if(decl->is_static)
	{
		if(!decl->initialize_expression) { assert(0 && "static declaration may not have no initlialize exression\n"); }		
	}

	if(decl->initialize_expression)
	{
		jo_sema_resolve_expr(sema, outer_scope, decl->initialize_expression);
		decl_node->resolved_type = decl->initialize_expression->resolved_type;
	}

	if(decl->specified_type)
	{
		if(decl->initialize_expression)
		{
			if(!jo_sema_types_are_equal(decl->specified_type, decl->initialize_expression->resolved_type))
			{
				jo_astr_t spec_type_str = jo_sema_type_astr(sema->arena, decl->specified_type);
				jo_astr_t resolved_type_str = jo_sema_type_astr(sema->arena, decl->initialize_expression->resolved_type);
			
				printf("declaration of %.*s has specified type %.*s but has initilize expression type %.*s\n", 
					jo_str_view_fmt(&decl->identifier->data.identifier), 
					jo_astr_fmt(&spec_type_str),
					jo_astr_fmt(&resolved_type_str)
				);
				assert(0);
				
				// if (decl->specified_type->type == jo_ast_type_type_primitive && 
				// 	decl->initialize_expression->resolved_type->type == jo_ast_type_type_primitive)
				// {
				// 	if(!jo_is_primitive_type_implicitly_castable_to(
				// 		decl->specified_type->data.type_primitive, 
				// 		decl->initialize_expression->resolved_type->data.type_primitive))
				// 	{
				// 		jo_string spec_type_str = jo_sema_type_string(decl->specified_type);
				// 		jo_string resolved_type_str = jo_sema_type_string(decl->initialize_expression->resolved_type);
			
				// 		printf("specified declaration type of %s(%s) has initilize expression type %s\n that is not implicitly castable to it", decl->identifier->data.identifier.data, spec_type_str.data, resolved_type_str.data);
			
				// 		jo_string_free(&spec_type_str);
				// 		jo_string_free(&resolved_type_str);
				// 		assert(0);
				// 	}
				// 	else
				// 	{
				// 		jo_ast_node_t* implicit_cast_node = jo_ast_node_make(sema->arena,jo_ast_type_expr_as_cast);
				// 		implicit_cast_node->data.expr_as_cast.expr = decl->initialize_expression;
				// 		implicit_cast_node->data.expr_as_cast.to_type = decl->specified_type;
				// 		implicit_cast_node->data.expr_as_cast.implicit = true;						
				
				// 		decl->initialize_expression = implicit_cast_node;
	
				// 	}
				// }							
			}
				
		}
		
		decl_node->resolved_type = decl->specified_type;
	}
}

bool jo_sema_analyze(jo_sema_t* sema, jo_ast_node_t* module)
{
	jo_sema_analyze_module(sema, &sema->global_scope, module);

	if(sema->errors.occupied > 0)
	{
		return false;
	}

	return true;
}
