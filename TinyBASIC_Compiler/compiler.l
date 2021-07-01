%option noyywrap yylineno

%{
extern "C" {
	#include <stdlib.h>
	#include <string.h>
	#include "compiler.tab.h"
	void yyerror(const char*);
}
#define YY_DECL extern "C" int yylex()
char buff[256];
char *s;
%}

%x S_STRING

digit	[0-9]
ch		[a-zA-Z]

%%

{digit}+				{ 
							yylval.integer = atoi(yytext);
							return INTEGER;
						}
{digit}+\.{digit}+		{
							yylval.decimal = atof(yytext);
                            return T_DECIMAL;
						}
[-+*\/(),]				{
							return *yytext;
						}
(=)						{	return EQ; }
(>)						{	return GT; }
(<)						{	return LT; }
(>=)					{	return GE; }
(<=)					{	return LE; }
(<>)|(><)				{	return NE; }
(\n)					{	return CR; }
(PRINT)					{	return PRINT; }
(IF)					{	return IF; }
(THEN)					{	return THEN; }
(GOTO)					{	return GOTO; }
(LET)					{	return LET; }
(END)					{	return END; }
(\")					{ BEGIN S_STRING; s = buff; 
						  *s++ = '\"'; 
						}
<S_STRING>\\\"			{ *s++ = '\\'; *s++ = '\"'; }
<S_STRING>\"			{ *s++ = '\"'; *s++ = 0;
						  yylval.str = _strdup(buff);
						  BEGIN 0;
						  return STRING;
						}
<S_STRING>.				{ *s++ = *yytext; }
{ch}+[{ch}0-9_]*		{	yylval.str = _strdup(yytext);
							return VARIABLE;
						}

[ \t]+					;
.						{
							return *yytext;
						}

%%
