/*
 * An integer stack implementation. The stack is constructed as a doubly linked
 * list. Internally, the tail is used as the point of addition and removal of
 * nodes, making it (accidentally) different from textbook implementations.
 * Use new_stack() to instantiate a stack.
 */

#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

void log_error(const char *function_name, const char *message) {
    fprintf(stderr, "stack.c: Internal error in %s(): %s\n", function_name,
        message);
}

/*
 * Intended constructor for Stacks. Do not initialize a Stack with malloc.
 */
Stack *new_stack() {
    Stack *stack = malloc(sizeof(Stack));
    stack->head = NULL;
    stack->tail = NULL;
    stack->size = 0;
    return stack;
}

/*
 * Returns the current size of the stack (the number of items).
 */
int stack_size(Stack *stack) {
    if (stack == NULL) {
        log_error("stack_size", "stack is NULL");
    }
    return stack->size;
}

/*
 * Returns the tail's value. Fails if the stack is empty.
 */
int stack_peek(Stack *stack) {
    if (stack == NULL) {
        log_error("stack_peek", "stack is NULL");
    } else if (stack->tail == NULL) {
        log_error("stack_peek", "tail is NULL");
    }
    return stack->tail->val;
}

/*
 * Adds a value to the stack.
 */
void stack_push(Stack *stack, int val) {
    if (stack == NULL) {
        log_error("stack_push", "stack is NULL");
    }
    Node *new_node = malloc(sizeof(Node));
    new_node->val = val;
    if (stack->head == NULL) {
        stack->head = new_node;
        stack->tail = new_node;
        new_node->prev = NULL;
        new_node->next = NULL;
    } else {
        stack->tail->next = new_node;
        new_node->prev = stack->tail;
        stack->tail = new_node;
        new_node->next = NULL;
    }
    stack->size += 1;
}

/*
 * Removes a value from the stack. Fails if the stack is empty.
 */
int stack_pop(Stack *stack) {
    if (stack == NULL) {
        log_error("stack_pop", "stack is NULL");
    }
    if (stack->size == 0) {
        log_error("stack_pop", "stack is empty");
    }
    int ret = stack->tail->val;
    if (stack->size == 1) {
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

/*
 * Frees all pointer references within the stack, and the stack itself.
 */
void stack_free(Stack *stack) {
    if (stack == NULL) {
        return;
    }
    Node *curr = stack->head;
    while (curr != NULL) {
        Node *temp = curr; 
        curr = curr->next;
        free(temp);
    }
    free(stack);
}
