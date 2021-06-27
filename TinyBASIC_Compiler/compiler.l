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

%x SSTRING

digit	[0-9]
ch		[a-zA-Z]

%%

{digit}+				{ 
							yylval.integer = atoi(yytext);
							return INTEGER;
						}
{digit}+.{digit}+		{
							yylval.decimal = atof(yytext);
							return DECIMAL;
						}
[-+*/(),]				{
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
(INPUT)					{	return INPUT; }
(LET)					{	return LET; }
(END)					{	return END; }
(\")					{ BEGIN SSTRING; s = buff; 
						  *s++ = '\"'; 
						}
<SSTRING>\\\"			{ *s++ = '\\'; *s++ = '\"'; }
<SSTRING>\"				{ *s++ = '\"'; *s++ = 0;
						  yylval.str = _strdup(buff);
						  BEGIN 0;
						  return STRING;
						}
<SSTRING>.				{ *s++ = *yytext; }
{ch}+					{	yylval.str = _strdup(yytext);
							return VARIABLE;
						}

[ \t]+					;
.						{
							return *yytext;
						}

%%