#include <stdio.h>

#define MAXSIZE 1000
typedef struct {
    char buff[MAXSIZE];
    int top;
}Stack;

void stack_init(Stack *p_stack);

int stack_is_full(Stack *p_stack);

int stack_is_empty(Stack *p_stack);

int stack_push(Stack *p_stack, char data);

int stack_pop(Stack *p_stack, char *p_data);
