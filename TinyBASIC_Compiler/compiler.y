%define parse.error verbose

%{
extern "C" {
	#include "compiler.h"
	#include "syntax_tree.h"
	int yylex();
	int YYACCEPT();
}
// Symbol table for user variables
SymTable* table = symtbl_create(1);
void yyerror(const char*);
extern int yy_flex_debug;
extern int yylineno;
extern FILE* yyin;
// Track lineno (increments by default, or can be set by input)
int lineno = -1;
// Output file for compiled code
FILE* input;
FILE* out;
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
	line_list END					{ fprintf(out, "\treturn 0;\n}"); YYACCEPT; }
	| END							{ fprintf(out, "\treturn 0;\n}"); YYACCEPT; }
	;

line_list:
	line_list line
	| line
	;

line:
	INTEGER statement CR			{ lineno = $1;
									 trans(out, table, $2, lineno);
									}
	| statement CR					{ 
									 trans(out, table, $1, -1);
									}
	| CR							{}
	;

statement:
	PRINT expr_list										{ $$ = n_op(PRINT, 1, $2); }
	| IF expression relop expression THEN statement		{ $$ = n_op(IF, 4, $2, $3, $4, $6); }
	| GOTO expression									{ $$ = n_op(GOTO, 1, $2); }
	| LET var EQ expression								{ $$ = n_op(LET, 2, $2, $4); }
	| LET var EQ string									{ $$ = n_op(LET, 2, $2, $4); }
	;

expr_list:
	expr_list ',' string			{ $$ = n_op('e', 2, $1, $3); }
	| expr_list ',' expression		{ $$ = n_op('e', 2, $1, $3); }
	| string						{ $$ = $1; }
	| expression					{ $$ = $1; }
	;

var_list:
	var_list ',' var				{ $$ = n_op('v', 2, $1, $3); }
	| var							{ $$ = $1; }
	;

expression:
	expression '+' term				{ $$ = n_op('+', 2, $1, $3); }
	| expression '-' term			{ $$ = n_op('-', 2, $1, $3); }
	| '-' expression %prec UMINUS	{ $$ = n_op(UMINUS, 1, $2); }
	| term							{ $$ = $1; }
	;

term:
	term '*' factor			{ $$ = n_op('*', 2, $1, $3); }
	| term '/' factor		{ $$ = n_op('/', 2, $1, $3); }
	| factor				{ $$ = $1; }
	;

factor:
	var						{ $$ = $1; }
	| number				{ $$ = $1; }
	| '(' expression ')'	{ $$ = n_op('(', 1, $2); }
	;

string:
	STRING			{ $$ = n_str($1); }
	;

var:
	VARIABLE	{ $$ = n_var($1); }
	;

number:
	INTEGER		{ $$ = n_int($1); }
	| T_DECIMAL	{ $$ = n_float($1); }
	;

relop:
	EQ			{ $$ = n_op(EQ, 0); }
	| GT		{ $$ = n_op(GT, 0); }
	| LT		{ $$ = n_op(LT, 0); }
	| GE		{ $$ = n_op(GE, 0); }
	| LE		{ $$ = n_op(LE, 0); }
	| NE		{ $$ = n_op(NE, 0); }
	;

%%

void yyerror(const char* msg){
	fprintf(stderr, "ERROR (Line %d): %s\n", yylineno, msg);
}

int main(int argc, char **argv){
	// Attempt to assign passed input and output files
	if (argc > 2){
		input = fopen(argv[1], "r");
		if (!input) fprintf(stderr, "Failed to open input file \"%s\".", argv[1]);
		yyin = input;
		out = fopen(argv[2], "w");
		if (!out) fprintf(stderr, "Failed to open output file \"%s\".", argv[1]);
	}
	else {
		fprintf(stderr, "Insufficient args. Using default input and output.");
		input = fopen("source.tb", "r");
		if (!input) yyin = stdin;
		else yyin = input;
		out = fopen("source.c", "w");
		if (!out) out = stdout;
	}
	// Append compiler header
	fprintf(out, "#include <stdlib.h>\n#include <stdio.h>\n#include <string.h>\nint main(){\n");
	// Specify debug modes
	yy_flex_debug = 0;
	//yydebug = 0;
	yyparse();
	return 0;
}