/**
******************************************************************************
* @file    syntax_tree.c 
* @author  Ben Lewis
* @version V1.0.0
* @date    03-July-2021
* @brief   Provide support for the creation and manipulation of a syntax tree.
******************************************************************************
*/

#include "syntax_tree.h"

/** @addtogroup TB_COMPILER
* @{
*/

/** @defgroup SYNTAX_TREE
* @brief syntax tree definitions file
* @{
*/ 

/** @defgroup SYNTAX_TREE_Functions
* @{
*/ 

/**
* @brief Create an empty node in memory.
* @param None
* @retval NodeWrapper* created.
*/
NodeWrapper* n_create(){
	NodeWrapper* node = (NodeWrapper*) malloc(sizeof(NodeWrapper));
	if (!node){
		// TODO: convert all stderr to yyerror
		fprintf(stderr, "Failed to allocated memory for NodeWrapper.\n");
		exit(-1);
	}
	return node;
}

/**
* @brief Create a node of type int in memory.
* @param: val the integer value the node represents
* @retval NodeWrapper* created.
*/
NodeWrapper* n_int(int val){
	NodeWrapper* node = n_create();
	node->type = NodeInt;
	node->raw = RawInt;
	node->v_int = val;
	return node;
}

/**
* @brief Create a node of type float in memory.
* @param val: the float value the node represents
* @retval NodeWrapper* created.
*/
NodeWrapper* n_float(double val){
	NodeWrapper* node = n_create();
	node->type = NodeFloat;
	node->raw = RawFloat;
	node->v_float = val;
	return node;
}

/**
* @brief Create a node of type string in memory.
* @param val: the string value the node represents
* @retval NodeWrapper* created.
*/
NodeWrapper* n_str(char* val){
	NodeWrapper* node = n_create();
	node->type = NodeStr;
	node->raw = RawStr;
	node->v_str = malloc( (strlen(val)+1) * sizeof(char));
	strcpy(node->v_str, val);
	return node;
}

/**
* @brief Create a node of type variable in memory. Note: Does NOT create a symbol, only a reference to a symbol in the syntax tree.
* @param table: the symbol table
* @retval NodeWrapper* created.
*/
NodeWrapper* n_var(SymTable* table, char* id){
	NodeWrapper* node = n_create();
	node->type = NodeVar;

	// If symbol of same ID exists, copy raw value TODO removed because this assumes user as assigning same type to the var, not garuanteed H: <- that assumes user is even assigning
	Symbol* existing = symtbl_get(table, id);
	if (existing) node->raw = existing->node->raw;
	else node->raw = RawUnknown;
	
	node->v_var = malloc( (strlen(id)+1) * sizeof(char));
	strcpy(node->v_var, id);
	return node;
}

/**
* @brief Create a node of type operator in memory.
* @param table: the symbol table
* @param oper: operator token value
* @param nops: number of operands
* @retval NodeWrapper* created.
*/
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
				// TODO commented 02/07/21 19:15
				/*if (sym->node->raw == RawFloat) 
					node->raw = RawFloat;*/
				node->raw = sym->node->raw;
			}
		}
		
		// Any operand being a float requires the raw value is a float
		if (node->v_op.operands[i]->raw == RawFloat) raw = RawFloat;
	}
	va_end(args);
	node->raw = raw;
	return node;
}

/**
* @brief Free memory allocated to a node.
* @param None
* @retval None
*/
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

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/ 