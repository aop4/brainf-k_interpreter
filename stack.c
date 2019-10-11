#include <stdlib.h>
#include "stack.h"

int stack_size(Stack *stack) {
    return stack->size;
}

int peek(Stack *stack) {
    return stack->tail->val;
}

void push(Stack *stack, int val) {
    Node *new_node = malloc(sizeof(Node));
    new_node->val = val;
    if (stack->head == NULL) {
        stack->head = new_node;
        stack->tail = new_node;
    } else {
        stack->tail->next = new_node;
        new_node->prev = stack->tail;
        stack->tail = new_node;
    }
    stack->size += 1;
}

int *pop(Stack *stack) {
    if (stack->size == 0) {
        return NULL;
    }
    int *ret = malloc(sizeof(int));
    *ret = stack->tail->val;
    if (stack->size == 1) {
        free(stack->head);
        free(stack->tail);
        stack->head = NULL;
        stack->tail = NULL;
    } else {
        Node *new_tail = stack->tail->prev;
        free(stack->tail);
        new_tail->next = NULL;
        stack->tail = new_tail;
    }
    stack->size -= 1;
    return ret;
}
