#include "sema.h"
#include "ast_node.h"
#include "parser.h"
#include "symbol.h"

#include <assert.h>
#include <stdio.h>

#define sema_resolve_primitive_type_case(type)\
case ast_type_literal_##type:\
	{\
		ast_node* type_node = ast_node_make(&sema->ws->arena, ast_type_type_primitive);\
		type_node->data.type_primitive = tok_##type;\
		node->resolved_type = type_node;\
		break;\
	}

bool8 type_is_pointer(ast_node* type)
{
	return type->type == ast_type_type_ptr;
}

u32 get_primitive_type_size(tok type)
{
	u32 size = 0;
	switch (type)
	{
	case tok_i64:
	case tok_u64:
	case tok_f64:
		size = 8;
		break;

	case tok_i32:
	case tok_u32:
	case tok_f32:
		size = 4;
		break;

	case tok_i16:
	case tok_u16:
		size = 2;
		break;

	case tok_i8:
	case tok_u8:
	case tok_bool:
		size = 1;
		break;

	case tok_void:
		size = 0;
		break;
	
	default:
		assert(0);
		break;
	}

	return size;
}

str sema_type_str(arena* arena, ast_node* t1)
{
	if(!t1) assert(0);

	switch (t1->type)
	{
	case ast_type_type_primitive:
		return str_from_cstr(arena, tok_to_string(t1->data.type_primitive) + 7); // @explain: 7 is to remove tok_* prefix from tok stirng
		break;

	case ast_type_type_fn:
	{
		ast_type_fn* type_fn = &t1->data.type_fn;
		str out = str_from_cstr(arena, "fn(");
	
		for(uz param_i = 0; param_i < t1->data.type_fn.parameters.occupied; param_i++)
		{
			str param_str = sema_type_str(arena, t1->data.type_fn.parameters.data[param_i]);
	
			str_append_str(arena, &out, &param_str);
	
			if(param_i != t1->data.type_fn.parameters.occupied - 1)
			{
				str_append_cstr(arena, &out, ", ");			
			}
		}
	
	
		str_append_cstr(arena, &out, ")");
	
		
		if(type_fn->return_type)
		{
			str_append_cstr(arena, &out, " -> ");
	
			str ret_str = sema_type_str(arena, t1->data.type_fn.return_type);
			str_append_cstr(arena, &out, ret_str.data);
		}
		
		return out;
		break;
	}


	

	default:
		assert(0);
		break;
	}
}

bool8 sema_type_is_void(ast_node* type_node)
{
	if(!type_node)
	{
		assert(0 && "unresolved type\n");
		return false;
	}

	if(type_node->type != ast_type_type_primitive)
	{
		return false;
	}
	else
	{
		return type_node->data.type_primitive == tok_void;
	}

}

bool8 sema_types_are_equal(ast_node* t1, ast_node* t2)
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

	if(t1->type == ast_type_type_primitive)
	{
		return t1->data.type_primitive == t2->data.type_primitive;
	}
	else if(t1->type == ast_type_type_fn)
	{
		ast_type_fn* fn1 = &t1->data.type_fn;
		ast_type_fn* fn2 = &t2->data.type_fn;

		if(fn1->parameters.occupied != fn2->parameters.occupied)
		{
			return false;
		}

		bool8 pass = true;

		for(uz arg_i = 0; arg_i < fn1->parameters.occupied; arg_i++)
		{
			pass = sema_types_are_equal(fn1->parameters.data[arg_i]->data.decl.specified_type, fn2->parameters.data[arg_i]->data.decl.specified_type);
			if(!pass) return false;
		}

		return sema_types_are_equal(fn1->return_type, fn2->return_type);
	}
	else if(t1->type == ast_type_type_ptr)
	{
		return sema_types_are_equal(t1->data.type_ptr.inner, t2->data.type_ptr.inner);
	}
	else if(t1->type == ast_type_type_ref)
	{
		return sema_types_are_equal(t1->data.type_ref.inner, t2->data.type_ref.inner);
	}
	else if(t1->type == ast_type_type_array)
	{
		if(t1->data.type_array.array_size_expression && t2->data.type_array.array_size_expression)
		{
			
		}
		return false;
	}
	else if(t1->type == ast_type_type_type)
	{
		return sema_types_are_equal(t1->data.type_type.type_node, t2->data.type_type.type_node);
	}

	return false;
}

void sema_diagnose_type_implic_castabability_to(ast_node* t1, ast_node* t2)
{
	if(sema_types_are_equal(t1, t2)) return;


}

void sema_resolve_expr_op_call(sema* sema, scope* outer_scope, ast_node* expr)
{
	if(expr->resolved_type) return;

	symbol* sym = scope_lookup_symbol(outer_scope, expr->data.expr_op_call.target->data.identifier);

	
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

	for(uz arg_i = 0; arg_i < expr->data.expr_op_call.arguments.occupied; arg_i++)
	{
		sema_resolve_expr(sema, outer_scope, expr->data.expr_op_call.arguments.data[arg_i]);
		if(!sema_types_are_equal(
			sym->ast_node->resolved_type->data.type_fn.parameters.data[arg_i], 
			expr->data.expr_op_call.arguments.data[arg_i]->resolved_type))
		{
			marker marker = arena_mark(&sema->ws->arena);

			str expr_str = sema_type_str(&sema->ws->arena, sym->ast_node->resolved_type->data.type_fn.parameters.data[arg_i]);
			str resolved_str = sema_type_str(&sema->ws->arena, expr->data.expr_op_call.arguments.data[arg_i]->resolved_type);
			printf("argument provided for %.*s call at index %llu has unexpected type, expected %.*s got %.*s",
				str_fmt(&sym->identifier),
				arg_i,
				str_fmt(&expr_str),
				str_fmt(&resolved_str)
			);

			arena_pop_to_marker( marker);

			assert(0);
		}
	}

	expr->resolved_type = sym->ast_node->resolved_type->data.type_fn.return_type;
}

ast_node* sema_insert_as_cast(sema* sema, [[maybe_unused]] scope* outer_scope, ast_node* replace_expr, ast_node* dest_type)
{
	ast_node* cast_node = ast_node_make(&sema->ws->arena, ast_type_expr_as_cast);
	cast_node->data.expr_as_cast.expr = replace_expr;
	cast_node->data.expr_as_cast.to_type = dest_type;
	cast_node->resolved_type = dest_type;
	return cast_node;
}

void sema_resolve_expr_op_binary_type(sema* sema, scope* outer_scope, ast_node* expr)
{
	if(expr->resolved_type) return;

	sema_resolve_expr(sema, outer_scope, expr->data.expr_op_binary.left_expression);
	sema_resolve_expr(sema, outer_scope, expr->data.expr_op_binary.right_expression);

	ast_node* left_expr_type = expr->data.expr_op_binary.left_expression->resolved_type;
	ast_node* right_expr_type = expr->data.expr_op_binary.right_expression->resolved_type;
	if(left_expr_type->type == ast_type_type_primitive && right_expr_type->type == ast_type_type_primitive)
	{
		if(left_expr_type->data.type_primitive == right_expr_type->data.type_primitive) { expr->resolved_type = left_expr_type; }
		else 		
		{
			tok uptype = tok_pick_primitive_upcast(left_expr_type->data.type_primitive, right_expr_type->data.type_primitive);
			if(left_expr_type->data.type_primitive == uptype)
			{
				ast_node* dest_t = ast_node_make(&sema->ws->arena, ast_type_type_primitive);
				dest_t->data.type_primitive = uptype;
				expr->data.expr_op_binary.right_expression = sema_insert_as_cast(
					sema, 
					outer_scope, 
					expr->data.expr_op_binary.right_expression, 
					dest_t
				);	
				expr->resolved_type = left_expr_type;
			}
			else
			{
				ast_node* dest_t = ast_node_make(&sema->ws->arena, ast_type_type_primitive);
				dest_t->data.type_primitive = uptype;
				expr->data.expr_op_binary.left_expression = sema_insert_as_cast(
					sema, 
					outer_scope, 
					expr->data.expr_op_binary.left_expression, 
					dest_t
				);
				expr->resolved_type = right_expr_type;			

			}
		}		
	}
	else
	{
		assert(0);
	}
}



void sema_resolve_expr_assigment(sema* sema, scope* outer_scope, ast_node* node)
{
	sema_resolve_expr(sema, outer_scope, node->data.expr_assignment.expression);

	symbol* sym = scope_lookup_symbol(outer_scope, node->data.expr_assignment.target->data.identifier);

	if(!sym)
	{
		printf("undeclared identifier %.*s", strv_fmt(&node->data.expr_assignment.target->data.identifier));
		assert(0);
	}


	if(!sema_types_are_equal(sym->ast_node->resolved_type, node->data.expr_assignment.expression->resolved_type))
	{
		marker marker = arena_mark(&sema->ws->arena);

		str gtsrt = sema_type_str(&sema->ws->arena, sym->ast_node->resolved_type);
		str rtstr = sema_type_str(&sema->ws->arena, node->data.expr_assignment.expression->resolved_type);
		printf("can not assign expression of a type %.*s to variable %.*s of type %.*s"
				, str_fmt(&rtstr)
				, strv_fmt(&node->data.expr_assignment.target->data.identifier)
				, str_fmt(&gtsrt)
			);

		arena_pop_to_marker( marker);

		assert(0);
	}

}

void sema_resolve_expr_as_cast(sema* sema, scope* outer_scope, ast_node* node)
{
	sema_resolve_expr(sema, outer_scope, node->data.expr_as_cast.expr);
	node->resolved_type = node->data.expr_as_cast.to_type;	
}

void sema_resolve_expr(sema* sema, scope* outer_scope, ast_node* node)
{
	if(node->resolved_type) return;

	switch (node->type)
	{
		sema_resolve_primitive_type_case(i64);
		sema_resolve_primitive_type_case(u64);
		sema_resolve_primitive_type_case(f64);

	case ast_type_literal_string:
	{
		ast_node* ptr_type_node = ast_node_make(&sema->ws->arena, ast_type_type_ptr);
		ptr_type_node->data.type_ptr.inner = ast_node_make(&sema->ws->arena,ast_type_type_primitive);
		ptr_type_node->data.type_ptr.inner->data.type_primitive = tok_u8;
		node->resolved_type = ptr_type_node;
		break;
	}

	case ast_type_literal_fn:
    {
        ast_node* type_node = ast_node_make(&sema->ws->arena,ast_type_type_fn);
        type_node->data.type_fn.return_type = node->data.literal_fn.return_type;

		da_foreach(&node->data.literal_fn.parameters)
		{
			da_append(&sema->ws->arena, &type_node->data.type_fn.parameters, (*node->data.literal_fn.parameters.it)->data.decl.specified_type);
		}

        node->resolved_type = type_node;
   	    break;
    }

	case ast_type_literal_struct:
    {
        ast_node* type_node = ast_node_make(&sema->ws->arena,ast_type_type_struct);

		da_foreach(&node->data.literal_struct.members)
		{
			sema_resolve_expr(sema, outer_scope, *node->data.literal_struct.members.it);																					
			da_append(&sema->ws->arena, &type_node->data.type_struct.members, (*node->data.literal_struct.members.it)->resolved_type);
		}

        node->resolved_type = type_node;
   	    break;
    }

	case ast_type_literal_bool:
	{
		ast_node* type_node = ast_node_make(&sema->ws->arena,ast_type_type_primitive);
		type_node->data.type_primitive = tok_bool;
		node->resolved_type = type_node;
		break;
	}

	case ast_type_decl:
	{
        sema_resolve_decl(sema, outer_scope, node);

		symbol sym = {0};
		sym.ast_node = node;
		sym.identifier = str_from_view(&sema->ws->arena, node->data.decl.identifier->data.identifier);		
		scope_add_symbol(&sema->ws->arena, outer_scope, sym);		

		//@TODO: maybe there is a better way 
		sema_resolve_expr(sema, outer_scope, node->data.decl.identifier);		
		break;
	}
	case ast_type_identifier:
	{
		symbol* sym = scope_lookup_symbol(outer_scope, node->data.identifier);
		if(!sym)
		{
			printf("undeclared identifier %.*s", strv_fmt(&node->data.identifier));
			assert(0);
		}
		
		node->resolved_type = sym->ast_node->resolved_type;
		node->resolved_symbol = sym;			
		break;
	}

	case ast_type_expr_as_cast:
		sema_resolve_expr_as_cast(sema, outer_scope, node);
		break;
	case ast_type_expr_op_binary:
		sema_resolve_expr_op_binary_type(sema, outer_scope, node);
		break;
	case ast_type_expr_op_unary:
		assert(0);
		break;
	case ast_type_expr_op_call:
		sema_resolve_expr_op_call(sema, outer_scope, node);
		break;
	default:
		assert(0);
		break;
	}
}

void sema_resolve_stmt(sema* sema, scope* outer_scope, ast_node* stmt, ast_node* literal_parent_fn_node)
{
	ast_literal_fn* literal_parent_fn = &literal_parent_fn_node->data.literal_fn;

	switch (stmt->type)
	{
	case ast_type_stmt_expr:
		sema_resolve_expr(sema, outer_scope, stmt->data.stmt_expr.expr);

		break;
	case ast_type_stmt_ifelse:
		// sema_insert_as_cast(sema, outer_scope, stmt->data.stmt_ifelse.condition, token_keyword_bool);
		sema_resolve_expr(sema, outer_scope, stmt->data.stmt_ifelse.condition);
		sema_resolve_block(sema, outer_scope, stmt->data.stmt_ifelse.true_block, literal_parent_fn_node);

		if(stmt->data.stmt_ifelse.tail_stmt_if)
		{
			sema_resolve_stmt(sema, outer_scope, stmt->data.stmt_ifelse.tail_stmt_if, literal_parent_fn_node);
		}
		else if(stmt->data.stmt_ifelse.false_block)
		{
			sema_resolve_block(sema, outer_scope, stmt->data.stmt_ifelse.false_block, literal_parent_fn_node);
		}
		break;

	case ast_type_stmt_return:
		if(!literal_parent_fn_node)
		{
			assert(0 && "non fn block cant have return statement");
		}
		
		if(stmt->data.stmt_return.expression)
		{						
			sema_resolve_expr(sema, outer_scope, stmt->data.stmt_return.expression);

			if(!sema_types_are_equal(stmt->data.stmt_return.expression->resolved_type, literal_parent_fn->return_type))
			{
				marker marker = arena_mark(&sema->ws->arena);

				str estr = sema_type_str(&sema->ws->arena, literal_parent_fn->return_type);					
				str gstr = sema_type_str(&sema->ws->arena, stmt->data.stmt_return.expression->resolved_type);

				printf("function return type %.*s does not match return expression type %.*s\n"
					, str_fmt(&estr)
					, str_fmt(&gstr)
				);

				arena_pop_to_marker( marker);

				assert(0);
			}
		}
		else
		{
			if(!sema_type_is_void(literal_parent_fn->return_type))
			{
				marker marker = arena_mark(&sema->ws->arena);

				str estr = sema_type_str(&sema->ws->arena, literal_parent_fn->return_type);

				printf("function return type %.*s cant have void return\n"
					, str_fmt(&estr)
				);
				
				arena_pop_to_marker( marker);

				assert(0);
			}
		}



		break;
	case ast_type_block:
		sema_resolve_block(sema, outer_scope, stmt, literal_parent_fn_node);

		break;
	default:
		assert(0);
		break;
	}
}

void sema_resolve_block(sema* sema, scope* outer_scope, ast_node* block, ast_node* literal_parent_fn_node)
{
	scope* fn_block_scope = scope_push(&sema->ws->arena, outer_scope, strv_from_cstr("block"));
	
	for(uz i = 0; i < block->data.block.statements.occupied; i++)
	{
		ast_node* stmt = block->data.block.statements.data[i];
		
		sema_resolve_stmt(sema, fn_block_scope, stmt, literal_parent_fn_node);
	}
}

void sema_analyze_literal_fn(sema* sema, scope* fn_scope, ast_node* literal_fn_node)
{	
	ast_literal_fn* literal_fn = &literal_fn_node->data.literal_fn;
	
	if(literal_fn->intrinsic) return;

    da_foreach(&literal_fn->parameters)
	{			
		ast_node* param_node = *literal_fn->parameters.it;
		sema_resolve_decl(sema, fn_scope, param_node);

		symbol sym = {0};
		sym.identifier = str_from_view(&sema->ws->arena, param_node->data.decl.identifier->data.identifier);

		sym.kind = symbol_kind_variable;
		sym.ast_node = param_node;
		param_node->resolved_symbol = scope_add_symbol(&sema->ws->arena,fn_scope, sym);
	}

	sema_resolve_block(sema, fn_scope, literal_fn->block, literal_fn_node);
}

// void sema_analyze_literal_struct(sema* sema, scope* outer_scope, ast_node* literal_struct_node)
// {

// }

void analyze_decl(sema* sema, scope* outer_scope, ast_node* decl_node)
{
	assert(decl_node->resolved_type);

	switch (decl_node->resolved_type->type)
	{
	case ast_type_type_fn:
	{
		scope* fn_scope = scope_push(&sema->ws->arena, outer_scope, decl_node->data.decl.identifier->data.identifier);
		sema_analyze_literal_fn(sema, fn_scope, decl_node->data.decl.initialize_expression);
		break;
	}

	case ast_type_type_struct:
		// sema_analyze_literal_struct(sema, outer_scope, decl_node->data.decl.initialize_expression);
		break;

	
	default:
		break;
	}
}

void sema_analyze_workspace(sema* sema, scope* outer_scope)
{
	da_foreach(&sema->ws->loaded_modules)
	{
		ast_file* module = &sema->ws->loaded_modules.it->file_node->data.file;
		
		// module-space declaration are position independent

		for(uz i = 0; i < module->content.occupied; i++)
		{
			ast_node* current_content = module->content.data[i];
	
			switch (module->content.data[i]->type)
			{
				case ast_type_decl:
				{
					sema_resolve_decl(sema, outer_scope, current_content);
	
					symbol sym = {0};
					sym.kind = symbol_kind_function;
					sym.ast_node = current_content;
					sym.identifier = str_from_view(&sema->ws->arena ,current_content->data.decl.identifier->data.identifier);				
					current_content->resolved_symbol = scope_add_symbol(&sema->ws->arena,outer_scope, sym);
					break;
				}
	
			default:
				break;
			}
		}
	}

	da_foreach(&sema->ws->loaded_modules)
	{
		ast_file* module = &sema->ws->loaded_modules.it->file_node->data.file;
		
		for(uz i = 0; i < module->content.occupied; i++)	
		{
			ast_node* current_content = module->content.data[i];

			switch (current_content->type)
			{
			case ast_type_decl:
				analyze_decl(sema, &sema->ws->global_scope, current_content);
				break;

			default:
				break;
			}
		}
	}
}



void sema_resolve_decl(sema* sema, scope* outer_scope, ast_node* decl_node)
{
    ast_decl* decl = &decl_node->data.decl;

	if(decl->is_static)
	{
		if(!decl->initialize_expression) { assert(0 && "static declaration may not have no initlialize exression\n"); }		
	}

	if(decl->initialize_expression)
	{
		sema_resolve_expr(sema, outer_scope, decl->initialize_expression);
		decl_node->resolved_type = decl->initialize_expression->resolved_type;
	}

	if(decl->specified_type)
	{
		if(decl->initialize_expression)
		{
			if(!sema_types_are_equal(decl->specified_type, decl->initialize_expression->resolved_type))
			{
				marker marker = arena_mark(&sema->ws->arena);

				str spec_type_str = sema_type_str(&sema->ws->arena, decl->specified_type);
				str resolved_type_str = sema_type_str(&sema->ws->arena, decl->initialize_expression->resolved_type);
			
				printf("declaration of %.*s has specified type %.*s but has initilize expression type %.*s\n", 
					strv_fmt(&decl->identifier->data.identifier), 
					str_fmt(&spec_type_str),
					str_fmt(&resolved_type_str)
				);

				arena_pop_to_marker( marker);
				assert(0);
				
				// if (decl->specified_type->type == ast_type_type_primitive && 
				// 	decl->initialize_expression->resolved_type->type == ast_type_type_primitive)
				// {
				// 	if(!is_primitive_type_implicitly_castable_to(
				// 		decl->specified_type->data.type_primitive, 
				// 		decl->initialize_expression->resolved_type->data.type_primitive))
				// 	{
				// 		string spec_type_str = sema_type_string(decl->specified_type);
				// 		string resolved_type_str = sema_type_string(decl->initialize_expression->resolved_type);
			
				// 		printf("specified declaration type of %s(%s) has initilize expression type %s\n that is not implicitly castable to it", decl->identifier->data.identifier.data, spec_type_str.data, resolved_type_str.data);
			
				// 		string_release(&spec_type_str);
				// 		string_release(&resolved_type_str);
				// 		assert(0);
				// 	}
				// 	else
				// 	{
				// 		ast_node* implicit_cast_node = ast_node_make(&sema->ws->arena,ast_type_expr_as_cast);
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

bool8 sema_analyze(sema* sema)
{
	sema_analyze_workspace(sema, &sema->ws->global_scope);

	if(sema->errors.occupied > 0)
	{
		return false;
	}

	return true;
}
