#include "syntax_tree.h"

NodeWrapper* n_create(){
	NodeWrapper* node = (NodeWrapper*) malloc(sizeof(NodeWrapper));
	if (!node){
		// TODO: convert all stderr to yyerror
		fprintf(stderr, "Failed to allocated memory for NodeWrapper.\n");
		exit(-1);
	}
	return node;
}

NodeWrapper* n_int(int val){
	NodeWrapper* node = n_create();
	node->type = NodeInt;
	node->raw = RawInt;
	node->v_int = val;
	return node;
}

NodeWrapper* n_float(double val){
	NodeWrapper* node = n_create();
	node->type = NodeFloat;
	node->raw = RawFloat;
	node->v_float = val;
	return node;
}

NodeWrapper* n_str(char* val){
	NodeWrapper* node = n_create();
	node->type = NodeStr;
	node->raw = RawStr;
	node->v_str = malloc( (strlen(val)+1) * sizeof(char));
	strcpy(node->v_str, val);
	return node;
}

// WARNING: Does NOT create a symbol, only a reference to a symbol in the syntax tree
//NodeWrapper* n_var(char* id){
NodeWrapper* n_var(SymTable* table, char* id){
	NodeWrapper* node = n_create();
	node->type = NodeVar;

	// If symbol of same ID exists, copy raw value TODO removed because this assumes user as assigning same type to the var, not garuanteed
	/*Symbol* existing = symtbl_get(table, id);
	if (existing) node->raw = existing->node->raw;*/

	node->raw = RawUnknown;
	node->v_var = malloc( (strlen(id)+1) * sizeof(char));
	strcpy(node->v_var, id);
	return node;
}

//NodeWrapper* n_op(int oper, int nops, ...){
NodeWrapper* n_op(SymTable* table, int oper, int nops, ...){
	NodeWrapper* node = n_create();
	node->type = NodeOp;
	node->v_op.oper = oper;
	node->v_op.nops = nops;
	// Allocate mem to hold array of pointers to operand nodes
	node->v_op.operands = malloc(nops * sizeof(NodeWrapper*));
	// Assign operand nodes (variable args list)
	va_list args;
	va_start(args, nops);
	// Assume int until float required
	RawType raw = RawInt;
	// Division op requires float
	if (oper == '/') raw = RawFloat;
	for (int i = 0; i < nops; i++){
		// Assign operand
		node->v_op.operands[i] = va_arg(args, NodeWrapper*);
		
		// If operand is a var
		if (node->v_op.operands[i]->type == NodeVar){
			// If var in symbol table
			struct Symbol* sym = symtbl_get(table, node->v_op.operands[i]->v_var);
			if (sym){
				// Set var nodes raw to that found in table
				node->v_op.operands[i]->raw = sym->node->raw;
				// If var is float, set raw to float
				if (sym->node->raw == RawFloat) 
					node->raw = RawFloat;
			}
		}
		
		// Any operand being a float requires the raw value is a float
		if (node->v_op.operands[i]->raw == RawFloat) raw = RawFloat;
	}
	va_end(args);
	node->raw = raw;
	return node;
}

void n_free(NodeWrapper* node){
	// Free any allocated pointers in union
	switch (node->type){
		case NodeOp:
			for (int i = 0; i < node->v_op.nops; i++){
				n_free(node->v_op.operands[i]);
			}
			break;
		case NodeStr:
			free(node->v_str);
			break;
		case NodeVar:
			free(node->v_var);
			break;
	}
	// Free node
	free(node);
}