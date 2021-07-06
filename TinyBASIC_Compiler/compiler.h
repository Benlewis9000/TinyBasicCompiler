/**
******************************************************************************
* @file    compiler.h 
* @author  Ben Lewis
* @version V1.0.0
* @date    06-July-2021
* @brief   Definitions for functions implemented in the source file. Includes
* for all required libraries and functions.
******************************************************************************
*/

#pragma once

#include <stdbool.h>
#include <stdarg.h>
#include "compiler.tab.h"
#include "sym_table.h"
#include "syntax_tree.h"

extern void yyerror(const char*);

void trans_printf(FILE* out, SymTable* table, NodeWrapper* node);

void trans(FILE* out, SymTable* table, NodeWrapper* node, int line);
