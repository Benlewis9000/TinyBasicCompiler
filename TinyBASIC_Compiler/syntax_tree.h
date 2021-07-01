/*
#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

typedef enum NodeType {
	NodeInt, NodeFloat, NodeStr, NodeVar, NodeOp
} NodeType;

typedef enum RawType {
	RawInt, RawFloat, RawStr, RawUnknown
} RawType;

typedef struct NodeWrapper NodeWrapper;

typedef struct Op{
	int oper;
	int nops;
	NodeWrapper** operands;
} Op;

struct NodeWrapper {
	// Node type
	NodeType type;
	// Raw value node eventually deduces to (int, float, string)
	RawType raw;
	// Value
	union {
		int v_int;
		double v_float;
		char* v_str;
		char* v_var;
		Op v_op;
	};
};

NodeWrapper* n_int(int val);

NodeWrapper* n_float(double val);

NodeWrapper* n_str(char* val);

NodeWrapper* n_var(char* id);

NodeWrapper* n_op(int oper, int nops, ...);

void n_free(NodeWrapper* node);
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

// Need to use SymTable from "sym_table.h"
NodeWrapper* n_op(SymTable* table, int oper, int nops, ...);

void n_free(NodeWrapper* node);
