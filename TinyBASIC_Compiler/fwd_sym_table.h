/**
******************************************************************************
* @file    fwd_sym_table.h
* @author  Ben Lewis
* @version V1.0.0
* @date    06-July-2021
* @brief   Forward header for sym_table.h to resolve circular dependencies.
* Declares Symbol and SymTable structs, includes required headers and
* declares functions to be implemented in source.
******************************************************************************
*/

#pragma once

#include <stdlib.h>
#include "fwd_syntax_tree.h"

/** @addtogroup TB_COMPILER
* @{
*/

/** @addtogroup SYM_TABLE
* @{
*/ 

/** @defgroup SYM_TABLE_Structs
* @{
*/ 

/**
 * @brief Symbol that can be stored in a Symbol table.
 */
typedef struct Symbol {
	char* id;			/**< Unique identifier for this Symbol. */
	NodeWrapper* node;	/**< Pointer to node this Symbol represents. */
} Symbol;

/**
 * @brief Symbol Table to dynamically store symbols.
 */
typedef struct SymTable {
	size_t capacity;	/**< Maximum number of Symbols this table can store. */
	size_t size;		/**< Current number of Symbols this table stores. */
	Symbol** data;		/**< An array of pointers to symbols. */
} SymTable;

/**
* @{
*/

/**
* @{
*/

/**
* @{
*/

Symbol* symtbl_get(SymTable* symtbl, char* id);