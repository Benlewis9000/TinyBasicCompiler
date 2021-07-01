#pragma once

#include <stdlib.h>
#include "fwd_syntax_tree.h"

typedef struct Symbol {
	char* id;
	NodeWrapper* node;
} Symbol;

typedef struct SymTable {
	size_t capacity;
	size_t size;
	Symbol** data;
} SymTable;

Symbol* symtbl_get(SymTable* symtbl, char* id);