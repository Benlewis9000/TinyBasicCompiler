#pragma once

#include "compiler.h"

CompilerState state = BEGIN;

// TODO: remove, now done in main yacc method
void header(FILE* out){
	fprintf(out, "#include <stdlib.h>\n#include <stdio.h>\n#include <string.h>\nint main(){\n");
}

void trans_printf(FILE*out, SymTable* table, NodeWrapper* node){
	// Deduce the raw type that will be printed
	RawType raw;
	if (node->type == NodeVar){
		// If node is a var, check symbol table for actual node and raw type
		Symbol* sym = symtbl_get(table, node->v_var);
		if (!sym) {
			// If no symbol found, report error
			yyerror("Variable not found.");
			return;
		}
		raw = sym->node->raw;
	}
	else {
		raw = node->raw;
	}
	// Check raw value node eventually leads to, not nodes actual value
	switch(raw){
		case RawInt:
			fprintf(out, "\tprintf(\"%cd\", (int)", '%');
			break;
		case RawFloat:
			fprintf(out, "\tprintf(\"%cf\", (double)", '%');
			break;
		case RawStr:
			fprintf(out, "\tprintf(\"%cs\", ", '%');
			break;
		default:
			// Do not printf if no value deduced
			return;
	}
	// Continue traversal to deduce value
	trans(out, table, node, -1);
	fprintf(out, ");\n");
}

void trans(FILE* out, SymTable* table, NodeWrapper* node, int line){
	// If line number provided
	if (line >= 0){
		char label[12];
		_itoa(line, label, 10);
		// Check if line has been used (stored as NULL symbol in table)
		if (!symtbl_get(table, label)){
			// Push line number
			symtbl_push_node(table, label, NULL);
			// Print label
			fprintf(out, "L%d:;", line);
		}
		else {
			// Generate error message, skip label
			char* msg = malloc(129 * sizeof(char));
			sprintf(msg, "Line \"%s\" already exists. Skipping...", label);
			yyerror(msg);
			free(msg);
		}
	}
	// Switch node type to deduce correct action
	switch(node->type){
		// Raw types (primitives)
		case NodeInt:
			fprintf(out, "%d", node->v_int);
			break;
		case NodeFloat:
			fprintf(out, "%f", node->v_float);
			break;
		case NodeStr:
			fprintf(out, "%s", node->v_str);
			break;
		case NodeVar:
			// If var is not in symbol table, report error
			if (!symtbl_get(table, node->v_var)){
				yyerror("Variable not found.");
				return;
			}
			// Prepend "_" to avoid collisions with reserved C keywords
			fprintf(out, "_%s", node->v_var);
			break;
		case NodeOp:
			switch(node->v_op.oper){
				// Operators
				case PRINT:
					// Switch child operand type
					switch(node->v_op.operands[0]->type){
						case NodeOp:
							// If expression list, continue traversal
							if (node->v_op.operands[0]->v_op.oper == 'e'){
								trans(out, table, node->v_op.operands[0], -1);
							}
							// Else prepare a printf statement for the node
							else {
								trans_printf(out, table, node->v_op.operands[0]);
							}
							break;
						default:
							trans_printf(out, table, node->v_op.operands[0]);
							break;
					}
					break;
				// Expr-list
				case 'e':
					if (node->v_op.operands[0]->v_op.oper == 'e'){
						trans(out, table, node->v_op.operands[0], -1);
					}
					else {
						trans_printf(out, table, node->v_op.operands[0]);
					}
					trans_printf(out, table, node->v_op.operands[1]);
					break;
				case IF:
					// if
					fprintf(out, "\tif(");
					// expression
					trans(out, table, node->v_op.operands[0], -1);
					// relop
					trans(out, table, node->v_op.operands[1], -1);
					// expression
					trans(out, table, node->v_op.operands[2], -1);
					// then
					fprintf(out, ") {\n");
					// statement
					trans(out, table, node->v_op.operands[3], -1);
					fprintf(out, "\t}\n");
					break;
				case GOTO:
					// goto
					fprintf(out, "\tgoto L");
					trans(out, table, node->v_op.operands[0], -1);
					fprintf(out, ";\n");
					break;
				case LET:
					// Update variable nodes raw type
					node->v_op.operands[0]->raw = node->v_op.operands[1]->raw;
					// If var not in sym table, unused, therefore need to declare type
					if(!symtbl_get(table, node->v_op.operands[0]->v_var)){
						switch(node->v_op.operands[1]->raw){
							case RawInt:
								fprintf(out, "\tint ");
								break;
							case RawFloat:
								fprintf(out, "\tfloat ");
								break;
							case RawStr:
								fprintf(out, "\tchar* ");
								break;
							default:
								// Assignee is invalid, report error and ignore
								yyerror("Syntax error, unable to assign given value.");
								return;
						}
						// Push symbol AFTER checking assignee is valid
						symtbl_push_node(table, node->v_op.operands[0]->v_var, node->v_op.operands[1]);
					}
					// else, var already declared
					fprintf(out, "\t");
					// variable
					trans(out, table, node->v_op.operands[0], -1);
					// =
					fprintf(out, " = ");
					// value
					trans(out, table, node->v_op.operands[1], -1);
					fprintf(out, ";\n");
					break;
				// Arithmetics
				case UMINUS:
					// Negate number
					fprintf(out, " - ");
					trans(out, table, node->v_op.operands[0], -1);
					break;
				case '/':
					trans(out, table, node->v_op.operands[0], -1);
					fprintf(out, " / ");
					trans(out, table, node->v_op.operands[1], -1);
					break;
				case '*':
					trans(out, table, node->v_op.operands[0], -1);
					fprintf(out, " * ");
					trans(out, table, node->v_op.operands[1], -1);
					break;
				case '+':
					trans(out, table, node->v_op.operands[0], -1);
					fprintf(out, " + ");
					trans(out, table, node->v_op.operands[1], -1);
					break;
				case '-':
					trans(out, table, node->v_op.operands[0], -1);
					fprintf(out, " - ");
					trans(out, table, node->v_op.operands[1], -1);
					break;
				// Relational operators
				case EQ:
					fprintf(out," == ");
					break;
				case LT:
					fprintf(out," < ");
					break;
				case GT:
					fprintf(out," > ");
					break;
				case LE:
					fprintf(out," <= ");
					break;
				case GE:
					fprintf(out," >= ");
					break;
				case NE:
					fprintf(out," != ");
					break;
				// Ordinance
				case '(':
					fprintf(out, "( ");
					trans(out, table, node->v_op.operands[0], -1);
					fprintf(out, " )");
					break;
			}
			break;
			
	}

}
