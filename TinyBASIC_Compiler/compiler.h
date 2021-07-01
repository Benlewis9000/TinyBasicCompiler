#pragma once

#include <stdbool.h>
#include <stdarg.h>
#include "compiler.tab.h"
#include "sym_table.h"
#include "syntax_tree.h"

extern void yyerror(const char*);

typedef enum CompilerState {
	BEGIN, TRANSPILING, FINISHED
} CompilerState;

extern CompilerState state;

void trans_printf(FILE* out, SymTable* table, NodeWrapper* node);

void trans(FILE* out, SymTable* table, NodeWrapper* node, int line);
