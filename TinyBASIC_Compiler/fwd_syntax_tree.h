/**
******************************************************************************
* @file    fwd_syntax_tree.h
* @author  Ben Lewis
* @version V1.0.0
* @date    06-July-2021
* @brief   Forward header for syntax_tree.h to resolve circular dependencies.
* Declares NodeType and RawType enums, Op and NodeWrapper structs, includes 
* required headers and declares functions to be implemented in source.
******************************************************************************
*/

#pragma once

/** @addtogroup TB_COMPILER
* @{
*/

/** @addtogroup SYNTAX_TREE
* @{
*/ 

/** @defgroup SYNTAX_TREE_Enums
* @{
*/ 

/**
 * The type of a node. Used to track what value is stored in a 
 * NodeWrappers union member.
 */
typedef enum NodeType {
	NodeInt, NodeFloat, NodeStr, NodeVar, NodeOp
} NodeType;

/**
 * The primitive value a node and it's children potentially deduce
 * to. Used to track the type of an operator or variable.
 */
typedef enum RawType {
	RawInt, RawFloat, RawStr, RawUnknown
} RawType;

/**
* @}
*/

typedef struct NodeWrapper NodeWrapper;

/** @defgroup SYNTAX_TREE_Structs
* @{
*/ 

/**
 * Operator struct.
 */
typedef struct Op{
	int oper;				/**< Operator type (token value). */
	int nops;				/**< Number of operands. */
	NodeWrapper** operands;	/**< The operands, an array of pointers to nodes. */
} Op;

/**
 * NodeWrapper struct. Represents a node in a syntax tree.
 */
struct NodeWrapper {
	NodeType type;			/**< The type of the node help by the wrapper. */
	RawType raw;			/**< The raw primitive type the node eventually deduces to. */
	union {					/**< Data pertaining the nodes type. */
		int v_int;
		double v_float;
		char* v_str;
		char* v_var;
		Op v_op;
	};
};

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/
