#include "stack.h"

void stack_init(Stack *p_stack)
{
    p_stack->top = -1;
}

int stack_is_full(Stack *p_stack)
{
    if (p_stack->top + 1 == MAXSIZE)
        return 1;
    return 0;
}

int stack_is_empty(Stack *p_stack)
{
    if (p_stack->top == -1)
        return 1;
    return 0;
}

int stack_push(Stack *p_stack, char data)
{
    if (stack_is_full(p_stack)) {
        printf("Stack is full.\n");
        return -1;
    }
    p_stack->top ++;
    p_stack->buff[p_stack->top] = data;
    return 0;
}

int stack_pop(Stack *p_stack, char *p_data)
{
    if (stack_is_empty(p_stack)) {
        printf("Stack is empty.\n");
        return -1;
    }
    *p_data = p_stack->buff[p_stack->top];
    p_stack->top --;
    return 0;
}

// int main()
// {
//     int ret;
//     Stack stack;
//     stack_init(&stack);
//     int i;
//     for (i = 0; i < 12; i ++) {
//         ret = stack_push(&stack, 'a'+i);
//         if (ret == 0)
//             printf("push %c\n", 'a'+i);
//     }
//     for (i = 0; i < 12; i++) {
//         char ch;
//         ret = stack_pop(&stack, &ch);
//         if (ret == 0)
//             printf("pop %c\n", ch);
//     }
//     return 0;
// }

