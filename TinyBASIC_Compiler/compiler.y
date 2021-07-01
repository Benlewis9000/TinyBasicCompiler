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
// Symbol table for user variables
SymTable* table = symtbl_create(1);
void yyerror(const char*);
extern int yy_flex_debug;
extern int yylineno;
extern FILE* yyin;
// Output file for compiled code
FILE* input;
FILE* out;
FILE* info;
bool success = true;
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

void yyerror(const char* msg){
	fprintf(info, "ERROR (Line %d): %s\n", yylineno, msg);
	success = false;
}

int main(int argc, char **argv){
	bool opened = true;
	// Attempt to assign passed input, output and info files
	if (argc == 4){
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
		info = fopen(argv[3], "w");
		if (!info){
			fprintf(stderr, "Failed to open info file \"%s\". (%d)\n", argv[3], errno);
			info = fopen("nul", "w");
		}
		if (!opened) return 0;
	}
	// Attempt to assign pass input and output, info goes to stdout
	else if (argc == 3){
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
		info = stdout;
		if (!opened) return 0;
	}
	// Else use defaults
	else {
		fprintf(stderr, "Insufficient args. Using default input and output.\n");
		input = fopen("source.tb", "r");
		if (!input) yyin = stdin;
		else yyin = input;
		out = fopen("source.c", "w");
		if (!out) out = stdout;
		info = fopen("info.txt", "w");
	}
	// Append compiler header
	fprintf(out, "#include <stdlib.h>\n#include <stdio.h>\n#include <string.h>\nint main(){\n");
	// Specify debug modes
	yy_flex_debug = 1;
	yydebug = 1;
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