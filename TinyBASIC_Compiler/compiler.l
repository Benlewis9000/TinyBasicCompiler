/**
******************************************************************************
* @file    compiler.l
* @author  Ben Lewis
* @version V1.0.0
* @date    06-July-2021
* @brief   This file provides input for Flex to generate the lexical analyser.
* Included are all tokens and their regular expression patterns for the lexer
* to match when input is passed. Functionality for dynamic string building is
* also included.
******************************************************************************
*/

%option noyywrap yylineno

%{
extern "C" {
	#include <stdlib.h>
	#include <string.h>
	#include "compiler.tab.h"
	void yyerror(const char*);
}
#define YY_DECL extern "C" int yylex()

/** @addtogroup TB_COMPILER
* @{
*/

/** @defgroup LEXER
* @brief symbol table definitions file
* @{
*/ 

/** @defgroup LEXER_Variables
* @{
*/ 

/**
 * Char buffer used to build strings.
 */
char* buff;
/**
 * Record size of buffer (sizeof not compatile with realloc).
 */
int buff_size;
/**
 * Pointer to chars in buffer used for writing.
 */
char* s;

/**
* @}
*/ 

void resize_buffer(char** buffer, char** p_buffer, int &size, int min_offset);
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
(\")					{ 
							// Enter string state
							BEGIN S_STRING; 
							// Initialise buffer memory
							int buff_init_size = 8;
							buff = (char*) malloc(buff_init_size * sizeof(char));
							buff_size = buff_init_size;
							if(!buff) yyerror("Could no allocate enough memory for new string.");
							// Write char
							s = buff; 
							*s++ = '\"'; 
						}
<S_STRING>\\\"			{	
							// Resize buffer as needed
							resize_buffer(&buff, &s, buff_size, 2);
							// Write chars
							*s++ = '\\'; 
							*s++ = '\"';
						}
<S_STRING>\"			{ 
							// Resize buffer as needed
							resize_buffer(&buff, &s, buff_size, 2);
							// Write chars (null terminate)
							*s++ = '\"'; 
							*s++ = 0;
							// Duplicate buffer into lexer value for syntax parser access
							yylval.str = _strdup(buff);
							// Free buffer
							free(buff);
							// Return to original state
							BEGIN 0;
							// Return token found
							return STRING;
						}
<S_STRING>.				{ 
							// Resize buffer as needed
							resize_buffer(&buff, &s, buff_size, 1);
							// Write char
							*s++ = *yytext;
						}
{ch}+[{ch}0-9_]*		{	
							yylval.str = _strdup(yytext);
							return VARIABLE;
						}

[ \t]+					;
.						{
							return *yytext;
						}

%%

/** @defgroup LEXER_Functions
* @{
*/ 

/**
 * @brief: Dynamically resize a string buffer and reassign pointers to preserve data, if the
 * distance between the allocated pointer and writing pointer is less than the minimal
 * acceptable offset.
 * @param buffer: pointer to memory for string
 * @param p_buffer: pointer to pointer writing to string
 * @param size: value recording the size of the buffer (since sizeof is not compatible with
 * dynamically reallocated memory)
 * @param min_offset: minimum acceptable offset between memory and writing pointer before
 * a resize is required
 */
void resize_buffer(char** buffer, char** writer, int &size, int min_offset){
	// Calculate offset between writer and buffer
	int offset = *writer - *buffer;
	// If less than minimum acceptable offset, resize
	if (size - offset < min_offset){
		size = size * 2;
		// Update pointers
		*buffer = (char*) realloc(*buffer, size * sizeof(char));
		*writer = *buffer + offset;
	}
	// Report error if resize unsuccesful
	if(!buffer) yyerror("Could no reallocate enough memory for string.");
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