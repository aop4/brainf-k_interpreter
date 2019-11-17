#ifndef STACK_HEADER
#define STACK_HEADER

typedef struct Node {
    int val;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int size;
} Stack;

Stack *new_stack();

void stack_push(Stack *stack, int val);

int stack_pop(Stack *stack);

int stack_peek(Stack *stack);

int stack_size(Stack *stack);

void stack_free(Stack *stack);

#endif
