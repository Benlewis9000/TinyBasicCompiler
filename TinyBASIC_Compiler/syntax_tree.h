/**
******************************************************************************
* @file    syntax_tree.h
* @author  Ben Lewis
* @version V1.0.0
* @date    06-July-2021
* @brief   Function declarations for NodeWrapper and syntax tree related
* functions.
******************************************************************************
*/


#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "fwd_syntax_tree.h"
#include "fwd_sym_table.h"

NodeWrapper* n_int(int val);

NodeWrapper* n_float(double val);

NodeWrapper* n_str(char* val);

NodeWrapper* n_var(SymTable* table, char* id);

NodeWrapper* n_op(SymTable* table, int oper, int nops, ...);

void n_free(NodeWrapper* node);
