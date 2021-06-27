#pragma once

#include "sym_table.h"

Symbol* symbol_create(char* id, NodeWrapper* node){
	// Allocate mem for symbol
	Symbol* sym = (Symbol*) malloc(sizeof(Symbol));
	// Allocate mem for id string, copy in arg
	sym->id = (char*) malloc( (strlen(id)+1) * sizeof(char) );
	strcpy(sym->id, id);
	sym->node = node;
	return sym;
}

void symbol_free(Symbol* symbol){
	free(symbol->id);
}

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

void symtbl_free(SymTable* symtbl){
	for (int i = 0; i < symtbl->size; i++){
		symbol_free(symtbl->data[i]);
	}
	free(symtbl->data);
	free(symtbl);
}

void symtbl_push(SymTable* symtbl, Symbol* symbol){
	// Double capacity if table is full
	if (symtbl->size >= symtbl->capacity) {
		symtbl_resize(symtbl, symtbl->capacity * 2);
	}
	// Add symbol, then increment size
	symtbl->data[symtbl->size++] = symbol;
}

void symtbl_push_node(SymTable* symtbl, char* id, NodeWrapper* node){
	// If id already in table, simply update node to point to
	Symbol* sym = symtbl_get(symtbl, id);
	if (sym){
		sym->node = node;
	}
	// Else create a new symbol and push to table
	else {
		symtbl_push(symtbl, symbol_create(id, node));
	}
}

int symtbl_size(SymTable* symtbl){
	return symtbl->size;
}

Symbol* symtbl_get(SymTable* symtbl, char* id){
	// Iterate symbols in table
	for (int i = 0; i < symtbl->size; i++){
		Symbol* sym = symtbl->data[i];
		if(strcmp(id, sym->id) == 0)
			return sym;
	}
	return NULL;
}