#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

#define True 1
#define False 0
typedef int bool;

typedef struct
{
	char symbol;
	int priority;
} Operator;

Operator opt[] = {
	{'+', 0},
	{'-', 0},
	{'*', 1},
	{'/', 1},
};

bool is_operator(char ch)
{
	int total = sizeof(opt)/sizeof(Operator);
	int i;
	if (ch == '(' || ch == ')')
		return True;
	for (i = 0; i < total; i++) {
		if (opt[i].symbol == ch) {
			return True;
		}
	}
	return False;
}

int get_operator_priority(char ch)
{
	int total = sizeof(opt)/sizeof(Operator);
	int i;
	for (i = 0; i < total; i++) {
		if (opt[i].symbol == ch) {
			return opt[i].priority;
		}
	}
	return -1;
}

bool is_number(char ch)
{
	if (ch >= '0' && ch <= '9')
		return True;
	return False;
}

int to_post_expression(const char *expression_str, char *post_expression_str)
{
	Stack stack;
	stack_init(&stack);
	const char *ptr = expression_str;
	char *post_ptr = post_expression_str;
	while (*ptr != '\0') {
		// printf("%c ", *ptr);
		if (is_number(*ptr)) {
			*post_ptr = *ptr;
			post_ptr ++;
			// printf("%c\n", *ptr);
		} else if (is_operator(*ptr)){
			char opt_ch = *ptr;
			char opt_ch_0 = '\0';
			if (opt_ch == ')') {
				while (1) {
					stack_pop(&stack, &opt_ch_0);
					if (opt_ch_0 == '(')
						break;
					*post_ptr = opt_ch_0;
					post_ptr ++;
					// printf("%c\n", opt_ch_0);
				}
			} else if (opt_ch == '(') {
				stack_push(&stack, opt_ch);
			} else {
				int priorty = get_operator_priority(opt_ch);
				bool is_first_time = True;
				while (!stack_is_empty(&stack)) {
					stack_pop(&stack, &opt_ch_0);
					stack_push(&stack, opt_ch_0);
					if (opt_ch_0 == '(')
						break;
					if (is_first_time) {
						int priorty0 = get_operator_priority(opt_ch_0);
						if (priorty >= priorty0)
							break;
						is_first_time = False;
					}
					stack_pop(&stack, &opt_ch_0);
					*post_ptr = opt_ch_0;
					post_ptr ++;
					// printf("%c\n", opt_ch_0);
				}
				stack_push(&stack, opt_ch);
			}
		}
		ptr ++;
		//printf("\n");
	}
	char opt_ch_0 = '\0';
	while (!stack_is_empty(&stack)) {
		stack_pop(&stack, &opt_ch_0);
		*post_ptr = opt_ch_0;
		post_ptr ++;
	}
	*post_ptr = '\0';
	return 0;
}

int arithmetic(const char *expression_str)
{
	int res;
	char post_expression_str[1000];
	to_post_expression(expression_str, post_expression_str);
	printf("post_expression_str:%s\n", post_expression_str);
	char *ptr = post_expression_str;
	int array[1000];
	int index = 0;
	while (*ptr != '\0') {
		if (is_number(*ptr)) {
			array[index ++] = *ptr - '0';
		} else if (is_operator(*ptr)) {
			char opt_ch = *ptr;
			int num1, num2, num;
			num2 = array[-- index];
			num1 = array[-- index];
			switch (opt_ch) {
				case '+':
					num = num1 + num2;
					break;
				case '-':
					num = num1 - num2;
					break;
				case '*':
					num = num1 * num2;
					break;
				case '/':
					num = num1 / num2;
					break;
			}
			printf("%d %c %d=%d\n", num1, opt_ch, num2, num);
			array[index ++] = num;
		}
		ptr ++;
	}
	res = array[0];
	return res;
}


// 只能进行1位整数的四则运算
int main(int argc, char const *argv[])
{
	char *expression_str = "4+(3-1*3)+8/2";
	int res = arithmetic(expression_str);
	printf("%s=%d\n", expression_str, res);
	return 0;
}

