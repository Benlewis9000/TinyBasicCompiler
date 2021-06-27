#pragma once

#include "expr_list.h"

typedef struct ExprList {
	size_t capacity;
	size_t size;
	float* data;
} ExprList;

/**
 * @brief Create a new expression list
 * @param capacity: maximum number of expressions the list can hold
 * @retval ExprList* pointing to created expression list
 */
ExprList* expr_list_create(int capacity) {
	// Allocate mem for list, check successful
	ExprList* list = (ExprList*)malloc(sizeof(ExprList));
	if (!list) {
		fprintf(stderr, "Failed to allocated memory for ExprList.\n");
		exit(-1);
	}
	// Allocate mem for list data, check successful
	list->data = (float*)calloc(capacity, sizeof(float));
	if (!list->data) {
		fprintf(stderr, "Failed to allocate memory for ExprList data, capacity %d.\n", capacity);
		exit(-1);
	}
	// Assign variables
	list->capacity = capacity;
	list->size = 0;
	return list;
}

/**
 * @brief Resize an existing expression list to a new size, copying existing data.
 * There is no guarantee existing data will fit into new list, thus trimming may occur.
 * @param list: existing list to resize data of
 * @param new_capacity: maximum capacity of new list
 * @retval None
 */
void expr_list_resize(ExprList* list, int new_capacity) {
	// Allocate memory for new data, check successful
	float* new_data = (float*)calloc(new_capacity, sizeof(float));
	if (!new_data) {
		fprintf(stderr, "Failed to allocate memory for ExprList data, capacity %d.\n", new_capacity);
		exit(-1);
	}
	// Iterate over original data, stopping when list ends or new capacity reached
	int i = 0;
	for (; (i < list->size) && (i < new_capacity); ++i) {
		// Assign new data corresponding original value
		*(new_data + i) = *(list->data + i);
	}
	// Free original data
	free(list->data);
	// Assign new data
	list->data = new_data;
	// Update capacity and size
	list->capacity = new_capacity;
	list->size = i;
}

/**
 * @brief Free list and internal data.
 * @param list: List to free
 * @retval None
 */
void expr_list_free(ExprList* list) {
	free(list->data);
	free(list);
}

/**
 * @brief Push an element to the end of the list. Resize the list if it is full.
 * @param list: List to add to
 * @param expr: Expression to push
 * @retval None
 */
void expr_list_push(ExprList* list, float expr) {
	// Double capacity if list is full
	if (list->size >= list->capacity) {
		expr_list_resize(list, list->capacity * 2);
	}
	// Add expression, then increment size
	*(list->data + list->size++) = expr;
}

/**
 * @brief Get the number of elements in the list.
 * @param list: List to check length of
 * @retval int Length of list
 */
int expr_list_length(ExprList* list) {
	return list->size;
}

/**
 * @brief Get a pointer to an element in the list at the given index.
 * Returns NULL if index is out of bounds.
 * @param list: List to get element from
 * @param index: Index to get element from
 * @retval float* pointer to the element, or NULL if index is out of bounds
 */
float* expr_list_get(ExprList* list, int index) {
	// If index is OOB, return NULL
	if (index >= list->size) {
		fprintf(stderr, "Index %d is out of bounds for list at %p.\n", index, list);
		return NULL;
	}
	// Return pointer to element at index
	return (list->data + index);
}