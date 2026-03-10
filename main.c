#include <stdio.h>
#include <stdlib.h>

#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "compiler/symbol.h"
#include "compiler/sema.h"
// #include "compiler/codegen.h"

void dump_tokens(jo_token_dyn_array_t* tokens)
{
	jo_dyn_array_iter(tokens, it, 
		{			
			switch (tokens->data[it].type)
			{
			case jo_token_identifier: 
			case jo_token_literal_integer:
			case jo_token_literal_string:
				printf("line: %u colum: %u type: %s (%s)\n", tokens->data[it].line, tokens->data[it].column, jo_token_type_to_string(tokens->data[it].type), tokens->data[it].content);
				break;

			default:
				printf("line: %u colum: %u type: %s\n", tokens->data[it].line,  tokens->data[it].column, jo_token_type_to_string(tokens->data[it].type));
				break;
			}
		}
	);
}

void dump_ast_internal(jo_ast_node_t* node, int indent);

void dump_ast_internal(jo_ast_node_t* node, int indent) 
{
    if (!node) return;

    // Print indentation
    for (int i = 0; i < indent; ++i) 
    {
        printf("  ");
    }

    // Print the current node type
    printf("[%s]", jo_ast_node_type_to_stirng(node->type));

    // Handle node-specific data and recurse into children
    switch (node->type) 
    {
        case jo_ast_entry:
            printf("\n");
            jo_dyn_array_iter(&node->data.entry.declarations, it, {
                dump_ast_internal(node->data.entry.declarations.data[it], indent + 1);
            });
            break;
            
        case jo_ast_type:
            printf(" (ptr: %d, ref: %d, arr: %d)\n", 
                   node->data.type.pointer, node->data.type.reference, node->data.type.array);
            dump_ast_internal(node->data.type.inner, indent + 1);
            if (node->data.type.array) {
                dump_ast_internal(node->data.type.array_size_expression, indent + 1);
            }
            break;

        case jo_ast_type_primitive:
            // Assuming jo_token_type_to_string exists globally since it's used in dump_tokens
            printf(" : %s\n", jo_token_type_to_string(node->data.type_primitive));
            break;

        case jo_ast_type_function:
            printf("\n");
            jo_dyn_array_iter(&node->data.type_function.parameters, it, {
                dump_ast_internal(node->data.type_function.parameters.data[it], indent + 1);
            });
            dump_ast_internal(node->data.type_function.return_type, indent + 1);
            break;

        case jo_ast_type_struct:
            printf("\n");
            jo_dyn_array_iter(&node->data.type_struct.members, it, {
                dump_ast_internal(node->data.type_struct.members.data[it], indent + 1);
            });
            break;

        case jo_ast_type_tuple:
            printf("\n");
            jo_dyn_array_iter(&node->data.type_tuple.entries, it, {
                dump_ast_internal(node->data.type_tuple.entries.data[it], indent + 1);
            });
            break;

        case jo_ast_literal_integer:
            printf(" : %d\n", node->data.literal_integer);
            break;

        case jo_ast_literal_string:
            printf(" : \"%s\"\n", node->data.literal_string);
            break;

        case jo_ast_literal_bool:
            printf(" : %s\n", node->data.literal_bool ? "true" : "false");
            break;

        case jo_ast_identifier:
            printf(" : %s\n", node->data.identifier);
            break;

        case jo_ast_declaration:
            printf("\n");
            dump_ast_internal(node->data.declaration.identifier, indent + 1);
            dump_ast_internal(node->data.declaration.construct, indent + 1);
            break;

        case jo_ast_construct_variable:
            printf("\n");
            dump_ast_internal(node->data.construct_variable.type, indent + 1);
            dump_ast_internal(node->data.construct_variable.initializing_expression, indent + 1);
            break;

        case jo_ast_construct_fn:
            printf("\n");
            dump_ast_internal(node->data.construct_fn.type, indent + 1);
            dump_ast_internal(node->data.construct_fn.body_expression, indent + 1);
            break;

        case jo_ast_unary_opperation:
            printf(" : %s\n", jo_token_type_to_string(node->data.operation_unary.operator_type));
            dump_ast_internal(node->data.operation_unary.expression, indent + 1);
            break;

        case jo_ast_binary_opperation:
            printf(" : %s\n", jo_token_type_to_string(node->data.operation_binary.operator_type));
            dump_ast_internal(node->data.operation_binary.left_expression, indent + 1);
            dump_ast_internal(node->data.operation_binary.right_expression, indent + 1);
            break;

        case jo_ast_apply_operation:
            printf(" : apply_type %s\n", jo_token_type_to_string(node->data.operation_apply.apply_type));
            dump_ast_internal(node->data.operation_apply.target, indent + 1);
            jo_dyn_array_iter(&node->data.operation_apply.arguments, it, {
                dump_ast_internal(node->data.operation_apply.arguments.data[it], indent + 1);
            });
            break;

        case jo_ast_expression_if:
            printf("\n");
            dump_ast_internal(node->data.expression_if.condition, indent + 1);
            dump_ast_internal(node->data.expression_if.true_branch, indent + 1);
            dump_ast_internal(node->data.expression_if.false_branch, indent + 1);
            break;

        case jo_ast_statement_expression:
            printf("\n");
            dump_ast_internal(node->data.statement_expression.expression, indent + 1);
            break;

        case jo_ast_statement_declaration:
            printf("\n");
            dump_ast_internal(node->data.statement_declaration.declaration, indent + 1);
            break;

        case jo_ast_statement_return:
            printf("\n");
            dump_ast_internal(node->data.statement_return.expression, indent + 1);
            break;

        case jo_ast_block:
            printf("\n");
            jo_dyn_array_iter(&node->data.block.statements, it, {
                dump_ast_internal(node->data.block.statements.data[it], indent + 1);
            });
            if (node->data.block.yield_expression) {
                dump_ast_internal(node->data.block.yield_expression, indent + 1);
            }
            break;

        default:
            printf("\n");
            break;
    }
}

void dump_ast(jo_ast_node_t* root) 
{
    dump_ast_internal(root, 0);
}


int main(int argc, char** argv)
{	
	if(argc < 2)
	{
		printf("prodive soruce file to parse");
		return 1;
	}

	bool dtokens = false;
	bool dast = false;

	for(jo_u32 i = 2; i < argc; i++)
	{
		if(strcmp(argv[i], "ast") == 0)
		{
			dast = true;
		}
		else if(strcmp(argv[i], "tokens") == 0)
		{
			dtokens = true;
		}
		else
		{
			printf("unrecognized argument: %s", argv[i]);
			return 1;
		}
	}
	
	jo_token_dyn_array_t tokens = jo_lex_file("jota_src/test.jo");
	if(dtokens)
	{
		dump_tokens(&tokens);
	}

	jo_parser_t parser = { .tokens = &tokens };
	jo_ast_node_t* ast_entry = jo_parse(&parser);
	if(dast)
	{
		dump_ast(ast_entry);
	}

	jo_sema_analyze(ast_entry);

}