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

void push(Stack *stack, int val);

int *pop(Stack *stack);

int peek(Stack *stack);

int stack_size(Stack *stack);

#endif
