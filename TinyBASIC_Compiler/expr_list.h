#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef struct ExprList ExprList;

ExprList* expr_list_create(int capacity);

void expr_list_resize(ExprList* list, int new_capacity);

void expr_list_free(ExprList* list);

void expr_list_push(ExprList* list, float expr);

int expr_list_length(ExprList* list);

float* expr_list_get(ExprList* list, int index);