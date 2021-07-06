/**
******************************************************************************
* @file    compiler.y
* @author  Ben Lewis
* @version V1.0.0
* @date    06-July-2021
* @brief   This file provides input for Bison to generate the syntax parser.
* Here the Tiny Basic grammar is defined. Production rules retrieve tokens
* from the lexer and build a stack that is reduced when a rule is matched. 
* Actions associated to that rule are then executed to build and abstract
* syntax tree and transpile it to a C code source file.
******************************************************************************
*/

%define parse.error verbose

%{
extern "C" {
	#include <errno.h>
	#include "compiler.h"
	#include "syntax_tree.h"
	#include "sym_table.h"
	int yylex();
	int YYACCEPT();
}

void yyerror(const char*);

/** @addtogroup TB_COMPILER
* @{
*/

/** @defgroup PARSER
* @brief symbol table definitions file
* @{
*/ 

/** @defgroup PARSER_Variables
* @{
*/ 

/**
 * Symbol table for user variables.
 */
SymTable* table = symtbl_create(1);

/**
* Debug flag for lexer.
*/
extern int yy_flex_debug;

/**
* Current input line number.
*/
extern int yylineno;

/**
* File lexer will read tokens from.
*/
extern FILE* yyin;

/**
* File containing tiny basic source code.
*/
FILE* input;

/**
* File to write transpiled C code to.
*/
FILE* out;

/**
* File to write process info to.
*/
FILE* info;

/**
* Flag for compilation success, may be set to false at any point.
*/
bool success = true;

/**
* @}
*/ 

%}

%union {
	int integer;
	double decimal;
	char* var;
	char* str;
	struct NodeWrapper* node;
};

%token <integer> INTEGER;
%token <decimal> T_DECIMAL;
%token <str> STRING;
%token <var> VARIABLE;

%type <node> relop number string var factor term expression var_list expr_list statement line

%token PRINT IF THEN GOTO LET CR END

%left '/' '*'
%left '+' '-'
%left EQ GT LT GE LE NE
%nonassoc UMINUS

%%

program:
	line_list END					{ YYACCEPT; }
	| END							{ YYACCEPT; }
	;

line_list:
	line_list line
	| line
	;

line:
	INTEGER statement CR			{
									 trans(out, table, $2, $1); n_free($2);
									}
	| statement CR					{ 
									 trans(out, table, $1, -1);	n_free($1);
									}
	| CR							{}
	;

statement:
	PRINT expr_list										{ $$ = n_op(table, PRINT, 1, $2); }
	| IF expression relop expression THEN statement		{ $$ = n_op(table, IF, 4, $2, $3, $4, $6); }
	| GOTO expression									{ $$ = n_op(table, GOTO, 1, $2); }
	| LET var EQ expression								{ $$ = n_op(table, LET, 2, $2, $4); }
	| LET var EQ string									{ $$ = n_op(table, LET, 2, $2, $4); }
	;

expr_list:
	expr_list ',' string			{ $$ = n_op(table, 'e', 2, $1, $3); }
	| expr_list ',' expression		{ $$ = n_op(table, 'e', 2, $1, $3); }
	| string						{ $$ = $1; }
	| expression					{ $$ = $1; }
	;

var_list:
	var_list ',' var				{ $$ = n_op(table, 'v', 2, $1, $3); }
	| var							{ $$ = $1; }
	;

expression:
	expression '+' term				{ $$ = n_op(table, '+', 2, $1, $3); }
	| expression '-' term			{ $$ = n_op(table, '-', 2, $1, $3); }
	| '-' expression %prec UMINUS	{ $$ = n_op(table, UMINUS, 1, $2); }
	| term							{ $$ = $1; }
	;

term:
	term '*' factor			{ $$ = n_op(table, '*', 2, $1, $3); }
	| term '/' factor		{ $$ = n_op(table, '/', 2, $1, $3); }
	| factor				{ $$ = $1; }
	;

factor:
	var						{ $$ = $1; }
	| number				{ $$ = $1; }
	| '(' expression ')'	{ $$ = n_op(table, '(', 1, $2); }
	;

string:
	STRING			{ $$ = n_str($1); }
	;

var:
	VARIABLE	{ $$ = n_var(table, $1); }
	;

number:
	INTEGER		{ $$ = n_int($1); }
	| T_DECIMAL	{ $$ = n_float($1); }
	;

relop:
	EQ			{ $$ = n_op(table, EQ, 0); }
	| GT		{ $$ = n_op(table, GT, 0); }
	| LT		{ $$ = n_op(table, LT, 0); }
	| GE		{ $$ = n_op(table, GE, 0); }
	| LE		{ $$ = n_op(table, LE, 0); }
	| NE		{ $$ = n_op(table, NE, 0); }
	;

%%

/** @defgroup PARSER_Functions
* @{
*/ 

/**
 * @brief: Report an error during compilation to info file (sets success flag to false).
 * @param msg: Error message
 * @retval None
 */
void yyerror(const char* msg){
	fprintf(info, "ERROR (Line %d): %s\n", yylineno, msg);
	success = false;
}

/**
 * @brief: Main entry point for Tiny Basic Compiler application. Arguments passed should
 * be paths to an input file, output file and info file. If paths are not found, default
 * values of "source.tb", "source.c" and "info.txt" will be used respectively.
 * @param argc: Number of arguments
 * @param argv: Array of strings (arguments)
 * @retval int: 0 for success, otherwise compilation failure.
 */
int main(int argc, char **argv){
	bool opened = true;
	// Attempt to assign passed input, output and info files via argv
	// Insufficent args, default file paths uses
	if (argc < 3){
		fprintf(stderr, "Insufficient args. Using default input and output.\n");
		input = fopen("source.tb", "r");
		if (!input) yyin = stdin;
		else yyin = input;
		out = fopen("source.c", "w");
		if (!out) out = stdout;
		info = fopen("info.txt", "w");
	}
	else {
		// Input and output file paths passed
		if (argc > 2){
			input = fopen(argv[1], "r+");
			if (!input) {
				fprintf(stderr, "Failed to open input file \"%s\". (%d)\n", argv[1], errno);
				opened = false;
			}
			yyin = input;
			out = fopen(argv[2], "w");
			if (!out) {
				fprintf(stderr, "Failed to open output file \"%s\". (%d)\n", argv[2], errno);
				opened = false;
			}
			info = stderr;
			if (!opened) return 0;
		}
		// Info file path passed
		if (argc > 3){
			info = fopen(argv[3], "w");
			if (!info){
				fprintf(stderr, "Failed to open info file \"%s\". (%d)\n", argv[3], errno);
				info = fopen("nul", "w");
			}
			if (!opened) return 0;
		}
	}

	// Append compiler header
	fprintf(out, "#include <stdlib.h>\n#include <stdio.h>\n#include <string.h>\nint main(){\n");
	// Specify debug modes
	yy_flex_debug = 0;
	yydebug = 0;
	yyparse();
	// Append compiler footer
	fprintf(out, "\treturn 0;\n}"); 
	// Report success
	if (success) {
		fprintf(info, "Compiled successfully.\n");
		return 0;
	}
	else {
		return 1;
	}
}

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/ 