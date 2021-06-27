#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "syntax_tree.h"

typedef struct Symbol {

	char* id;
	NodeWrapper* node;
	/*
	int type;	// 0 for int, 1 for string
	char* str;
	int val;
	*/
} Symbol;

typedef struct SymTable {
	size_t capacity;
	size_t size;
	Symbol** data;
} SymTable;

//Symbol* symbol_create(char* id, int type, char* str, int val);

Symbol* symbol_create(char* id, NodeWrapper* node);

//void symbol_free(Symbol* symbol);

//void symbol_print(Symbol* symbol);

SymTable* symtbl_create(int capacity);

void symtbl_resize(SymTable* symtbl, int new_capacity);

void symtbl_free(SymTable* symtbl);

void symtbl_push(SymTable* symtbl, Symbol* symbol);

void symtbl_push_node(SymTable* symtbl, char* id, NodeWrapper* node);

int symtbl_size(SymTable* symtbl);

Symbol* symtbl_get(SymTable* symtbl, char* id);