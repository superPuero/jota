#include <stdio.h>
#include <stdlib.h>

#include "compiler/compile_options.h"
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "compiler/symbol.h"
#include "compiler/sema.h"
#include "compiler/bytecode.h"
#include "compiler/vm.h"

void dump_tokens(jo_lexer_t* lexer)
{
	jo_token_dyn_array_t* tokens = &lexer->tokens;

	jo_dyn_array_iter(tokens, it,
		{
			switch (tokens->data[it].type)
			{
			case jo_token_identifier:
			case jo_token_literal_u64:
			case jo_token_literal_i64:
			case jo_token_literal_f32:
			case jo_token_literal_f64:
			case jo_token_literal_string:
				printf("line: %u colum: %u type: %s (%.*s)\n", tokens->data[it].line, tokens->data[it].column, jo_token_type_to_string(tokens->data[it].type), tokens->data[it].content_len, tokens->data[it].content);
				break;

			default:
				printf("line: %u colum: %u type: %s\n", tokens->data[it].line,  tokens->data[it].column, jo_token_type_to_string(tokens->data[it].type));
				break;
			}
		}
	);
}

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

static void dump_ast_internal(jo_ast_node_t* node, int indent) {
    if (!node) {
        print_indent(indent);
        printf("<NULL>\n");
        return;
    }

    print_indent(indent);

    printf("[%s]", jo_ast_node_type_to_stirng(node->type));

    switch (node->type)
	{		
        case jo_ast_type_literal_u64:
			printf(" %u\n", node->data.literal_u64);
			break;
        case jo_ast_type_literal_i64:
			printf(" %i\n", node->data.literal_i64);
			break;
        case jo_ast_type_literal_f32:
			printf(" %f\n", node->data.literal_f32);
		case jo_ast_type_literal_f64:
			printf(" %f\n", node->data.literal_f64);
			break;
        case jo_ast_type_literal_bool:
            printf(" %s\n", node->data.literal_bool ? "true" : "false");
            break;
		case jo_ast_type_literal_type:
            printf("\n");
			dump_ast_internal(node->data.literal_type.type, indent + 1);
            break;
        case jo_ast_type_literal_string:
            printf(" \"%s\"\n", node->data.literal_string.data);
            break;
        case jo_ast_type_identifier:
            printf(" %s\n", node->data.identifier.data);
            break;
        case jo_ast_type_type_primitive:
            printf(" (%s)\n", jo_token_type_to_string(node->data.type_primitive));
            break;

		case jo_ast_type_stmt_expr:
            printf("\n");
			dump_ast_internal(node->data.stmt_expr.expr, indent + 1);
			break;

		case jo_ast_type_module:
            printf("\n");
            jo_dyn_array_iter(&node->data.module.content, i, {
                dump_ast_internal(node->data.module.content.data[i], indent + 1);
            });
            break;
        case jo_ast_type_decl_namespace:
            printf("\n");
            jo_dyn_array_iter(&node->data.decl_namesapce.decls_and_directives, i, {
                dump_ast_internal(node->data.decl_namesapce.decls_and_directives.data[i], indent + 1);
            });
            break;

        case jo_ast_type_type_ptr:
            printf("\n");
            dump_ast_internal(node->data.type_ptr.inner, indent + 1);
            break;
        case jo_ast_type_type_ref:
            printf("\n");
            dump_ast_internal(node->data.type_ref.inner, indent + 1);
            break;
        case jo_ast_type_type_array:
            printf("\n");
            dump_ast_internal(node->data.type_array.inner, indent + 1);
            dump_ast_internal(node->data.type_array.array_size_expression, indent + 1);
            break;
        case jo_ast_type_type_fn:
            printf("\n");
            jo_dyn_array_iter(&node->data.type_fn.parameters, i, {
                dump_ast_internal(node->data.type_fn.parameters.data[i], indent + 1);
            });
            dump_ast_internal(node->data.type_fn.return_type, indent + 1);
            break;
        case jo_ast_type_type_tuple:
            printf("\n");
            jo_dyn_array_iter(&node->data.type_tuple.entries, i, {
                dump_ast_internal(node->data.type_tuple.entries.data[i], indent + 1);
            });
            break;

        case jo_ast_type_decl:
		    printf("\n");
            dump_ast_internal(node->data.decl.identifier, indent + 1);
            dump_ast_internal(node->data.decl.specified_type, indent + 1);
            dump_ast_internal(node->data.decl.initialize_expression, indent + 1);
            break;
        case jo_ast_type_literal_fn:
            printf("\n");
			jo_dyn_array_iter(&node->data.literal_fn.parameters, i, {
				dump_ast_internal(node->data.literal_fn.parameters.data[i], indent + 1);
            });
            dump_ast_internal(node->data.literal_fn.return_type, indent + 1);
            dump_ast_internal(node->data.literal_fn.block, indent + 1);
            break;
		case jo_ast_type_literal_struct:
            printf("\n");
			jo_dyn_array_iter(&node->data.literal_struct.members, i, {
				dump_ast_internal(node->data.literal_struct.members.data[i], indent + 1);
            });
	            break;

        // --- Expressions ---
        case jo_ast_type_expr_op_unary:
            printf(" (Op: %s)\n", jo_token_type_to_string(node->data.expr_op_unary.operator_type));
            dump_ast_internal(node->data.expr_op_unary.expression, indent + 1);
            break;
        case jo_ast_type_expr_op_binary:
            printf(" (Op: %s)\n", jo_token_type_to_string(node->data.expr_op_binary.operator_type));
            dump_ast_internal(node->data.expr_op_binary.left_expression, indent + 1);
            dump_ast_internal(node->data.expr_op_binary.right_expression, indent + 1);
            break;
        case jo_ast_type_expr_op_call:
            printf("\n");
            dump_ast_internal(node->data.expr_op_call.target, indent + 1);
            jo_dyn_array_iter(&node->data.expr_op_call.arguments, i, {
                dump_ast_internal(node->data.expr_op_call.arguments.data[i], indent + 1);
            });
            break;
		case jo_ast_type_expr_op_index:
            printf("\n");
            dump_ast_internal(node->data.expr_op_index.target, indent + 1);
            jo_dyn_array_iter(&node->data.expr_op_index.arguments, i, {
                dump_ast_internal(node->data.expr_op_index.arguments.data[i], indent + 1);
            });
            break;
        case jo_ast_type_stmt_ifelse:
            printf("\n");
            dump_ast_internal(node->data.stmt_ifelse.condition, indent + 1);
            dump_ast_internal(node->data.stmt_ifelse.true_block, indent + 1);
            dump_ast_internal(node->data.stmt_ifelse.tail_stmt_if, indent + 1);
            dump_ast_internal(node->data.stmt_ifelse.false_block, indent + 1);
            break;

        case jo_ast_type_expr_decl:
            printf("\n");
            dump_ast_internal(node->data.expr_decl.declaration, indent + 1);
            break;
		 case jo_ast_type_expr_as_cast:
            printf("\n");
            dump_ast_internal(node->data.expr_as_cast.expr, indent + 1);
            dump_ast_internal(node->data.expr_as_cast.to_type, indent + 1);
            break;
        case jo_ast_type_stmt_return:
            printf("\n");
            dump_ast_internal(node->data.stmt_return.expression, indent + 1);
            break;
        case jo_ast_type_stmt_for:
            printf("\n");
            dump_ast_internal(node->data.stmt_for.iterator, indent + 1);
            dump_ast_internal(node->data.stmt_for.iterable, indent + 1);
            dump_ast_internal(node->data.stmt_for.expression, indent + 1);
            break;
        case jo_ast_type_block:
            printf("\n");
            jo_dyn_array_iter(&node->data.block.statements, i, {
                dump_ast_internal(node->data.block.statements.data[i], indent + 1);
            });
            break;
  		case jo_ast_type_directive_load:
			printf(" %s\n", node->data.directive_load.path.data);
            break;

        default:
            printf(" <unmapped data>\n");
            break;
    }
}

void dump_ast(jo_ast_node_t* root)
{
    dump_ast_internal(root, 0);
}

void dump_bytecode(jo_bytecode_context* bcc)
{
    printf("-----------bytecode---------\n");
    printf("\n");

	jo_dyn_array_iter(&bcc->bc, j,
		{
			jo_bytecode_op* op = &bcc->bc.data[j];

			jo_dyn_array_iter(&bcc->fns, i,
				{
					if(j == bcc->fns.data[i].entry_ip)
					{
						printf("%s (registers: %u)\n", bcc->fns.data[i].label.data, bcc->fns.data[i].reg_counter);	
					}
				}
			);

			printf("%4zu: ", j);
			jo_bytecode_dump_op(bcc, op);
		}
	);
}

int main(int argc, char** argv)
{
	jo_arena_t arena = jo_arena_make(jo_Mb(1));

	jo_compile_options compile_opt = jo_compie_options_parse_from_args(argc, argv);	

	if(!compile_opt.success)
	{
		return 1;
	}

	if(compile_opt.tokens)
	{
		jo_lexer_t lexer = {0};
		lexer.arena = &arena; 
		jo_lex_file(&lexer, argv[1]);
		dump_tokens(&lexer);
	}

	//@Important: code repetition is intentional

	if(compile_opt.ast)
	{
		jo_lexer_t lexer = {.arena = &arena};
		jo_lex_file(&lexer, argv[1]);	
		jo_parser_t parser = { .arena = &arena, .lexer = &lexer };
		jo_ast_node_t* ast_module = jo_parse(&parser);
		dump_ast(ast_module);
	}

	if(compile_opt.sema)
	{
		jo_lexer_t lexer = {.arena = &arena};
		jo_lex_file(&lexer, argv[1]);	
		jo_parser_t parser = { .arena = &arena, .lexer = &lexer };
		jo_ast_node_t* ast_module = jo_parse(&parser);
		jo_scope_t s = {0};
		jo_sema_analyze(ast_module, &s);
	}

	if(compile_opt.bytecode)
	{
		jo_lexer_t lexer = {.arena = &arena};
		jo_lex_file(&lexer, argv[1]);	
		jo_parser_t parser = { .arena = &arena, .lexer = &lexer };
		jo_ast_node_t* ast_module = jo_parse(&parser);
		jo_scope_t s = {0};
		jo_sema_analyze(ast_module, &s);
		jo_bytecode_context bcc = jo_make_bytecode(&ast_module->data.module);
		dump_bytecode(&bcc);
	}


	if(compile_opt.interp)
	{
		jo_lexer_t lexer = {.arena = &arena};
		jo_lex_file(&lexer, argv[1]);	
		jo_parser_t parser = { .arena = &arena, .lexer = &lexer };
		jo_ast_node_t* ast_module = jo_parse(&parser);
		jo_scope_t s = {0};
		jo_sema_analyze(ast_module, &s);
		jo_bytecode_context bcc = jo_make_bytecode(&ast_module->data.module);
		jo_run_bytecode(&bcc);
	}

}
