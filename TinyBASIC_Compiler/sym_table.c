/**
******************************************************************************
* @file    sym_table.c 
* @author  Ben Lewis
* @version V1.0.0
* @date    06-July-2021
* @brief   Functions for the creation and manipulation of a symbol table. The
* symbol table may be used to store symbols, where a unique identifier 
* references a node in the syntax tree. Symbols may be used to represent
* variables provided in the user source code.
******************************************************************************
*/

#include "sym_table.h"

/** @addtogroup TB_COMPILER
* @{
*/

/** @defgroup SYM_TABLE
* @brief symbol table definitions file
* @{
*/ 

/** @defgroup SYM_TABLE_Functions
* @{
*/ 

/**
* @brief Create a Symbol in memory and assign values.
* @param id: string identifier for Symbol
* @param node: the node the Symbol refers to in syntax tree
* @retval Symbol* pointer to Symbol created.
*/
Symbol* symbol_create(char* id, NodeWrapper* node){
	// Allocate mem for symbol
	Symbol* sym = (Symbol*) malloc(sizeof(Symbol));
	// Allocate mem for id string, copy in arg
	sym->id = (char*) malloc( (strlen(id)+1) * sizeof(char) );
	strcpy(sym->id, id);
	sym->node = node;
	return sym;
}

/**
 * @brief Free a Symbol from memory.
 * @param None
 * @retval None
 */
void symbol_free(Symbol* symbol){
	free(symbol->id);
}

/**
* @brief Create a symbol table to dynamically store Symbols.
* @param capacity: initial maximum capcity of Symbols
* @retval SymTable* pointer to table created
*/
SymTable* symtbl_create(int capacity){
	// Allocate mem for table, check successful
	SymTable* symtbl = (SymTable*) malloc(sizeof(SymTable));
	if (!symtbl) {
		fprintf(stderr, "Failed to allocated memory for SymTable.\n");
		exit(-1);
	}
	// Allocate mem for table data, check successful
	symtbl->data = (Symbol**) malloc(capacity * sizeof(Symbol*));
	if (!symtbl->data) {
		fprintf(stderr, "Failed to allocate memory for SymTable data, capacity %d.\n", capacity);
		exit(-1);
	}
	// Assign variables
	symtbl->capacity = capacity;
	symtbl->size = 0;
	return symtbl;
}

/**
* @brief Safely resize the capacity of a symbol table whilst retaining data.
* @param symtbl: symbol table to resize
* @param new_capacity: capacity to resize to
* @retval None
*/
void symtbl_resize(SymTable* symtbl, int new_capacity){
	// Allocate memory for new data, check successful
	Symbol** new_data = (Symbol**)malloc(new_capacity * sizeof(Symbol*));
	if (!new_data) {
		fprintf(stderr, "Failed to allocate memory for SymTable data, capacity %d.\n", new_capacity);
		exit(-1);
	}
	// Iterate over original data, stopping when list ends or new capacity reached
	int i = 0;
	for (; (i < symtbl->size) && (i < new_capacity); ++i) {
		// Assign new data corresponding original value
		new_data[i] = symtbl->data[i];
	}
	// Free original data
	free(symtbl->data);
	// Assign new data
	symtbl->data = new_data;
	// Update capacity and size
	symtbl->capacity = new_capacity;
	symtbl->size = i;
}

/**
* @brief Free a symbol table and associated data from memory.
* @param symtbl: symbol table to free
* @retval None
*/
void symtbl_free(SymTable* symtbl){
	for (int i = 0; i < symtbl->size; i++){
		symbol_free(symtbl->data[i]);
	}
	free(symtbl->data);
	free(symtbl);
}

/**
* @brief Push a Symbol to a symbol table.
* @param symtbl: symbol table to push to
* @param symbol: pointer to Symbol to push
* @retval None
*/
void symtbl_push(SymTable* symtbl, Symbol* symbol){
	// Double capacity if table is full
	if (symtbl->size >= symtbl->capacity) {
		symtbl_resize(symtbl, symtbl->capacity * 2);
	}
	// Add symbol, then increment size
	symtbl->data[symtbl->size++] = symbol;
}

/**
* @brief Create a Symbol from a given node and id and push it to a symbo
* @param symtbl: symbol table to push to
* @param id: string identifier for new Symbol
* @param node: the node the new Symbol refers to in syntax tree
* @retval None
*/
void symtbl_push_node(SymTable* symtbl, char* id, NodeWrapper* node){
	// If id already in table, point sym to existing node
	Symbol* sym = symtbl_get(symtbl, id);
	if (sym){
		sym->node = node;
	}
	// Else create a new symbol and push to table
	else {
		symtbl_push(symtbl, symbol_create(id, node));
	}
}

/**
* @brief Get the number of Symbols stored in a given symbol table.
* @param symtbl: symbol table to get size of
* @retval int: the size of the table provided
*/
int symtbl_size(SymTable* symtbl){
	return symtbl->size;
}

/**
* @brief Given an ID, get a pointer to a Symbol inside a given table, if it is present.
* @param symtbl: symbol table to get Symbol from
* @param id: string identifier for the Symbol to get
* @retval Symbol*: pointer to corresponding Symbol, or NULL if none found
*/
Symbol* symtbl_get(SymTable* symtbl, char* id){
	// Iterate symbols in table
	for (int i = 0; i < symtbl->size; i++){
		Symbol* sym = symtbl->data[i];
		if(strcmp(id, sym->id) == 0)
			return sym;
	}
	return NULL;
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