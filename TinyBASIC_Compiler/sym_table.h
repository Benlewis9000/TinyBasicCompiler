/**
******************************************************************************
* @file    sym_table.h
* @author  Ben Lewis
* @version V1.0.0
* @date    06-July-2021
* @brief   Function declarations for Symbol and SymTable related functions.
******************************************************************************
*/

#pragma once

#include <stdio.h>
#include <string.h>
#include "fwd_sym_table.h"
#include "fwd_syntax_tree.h"

Symbol* symbol_create(char* id, NodeWrapper* node);

SymTable* symtbl_create(int capacity);

void symtbl_resize(SymTable* symtbl, int new_capacity);

void symtbl_free(SymTable* symtbl);

void symtbl_push(SymTable* symtbl, Symbol* symbol);

void symtbl_push_node(SymTable* symtbl, char* id, NodeWrapper* node);

int symtbl_size(SymTable* symtbl);
