#define _CRT_SECURE_NO_WARNINGS
#define SIZE 2000
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
typedef char StackElement;

typedef struct {
	double result;
	double prev_result;
}CalcData;

struct Stack {
	char str[SIZE + 1];
};

struct result {
	double data;
	char* error;
};

// Проинициализировать внутреннее содержимое структуры стека
void stack_init(struct Stack* s) {
	if (s == NULL) {
		return;
	}
	s->str[0] = '\0';
}

size_t stack_size(struct Stack* s) {
	if (!s) {
		return -1;
	}
	return strlen(s->str);
}

//Вернуть значение из верхушки стека
StackElement stack_peek(struct Stack* s) {
	if (s == NULL) {
		return -1;
	}
	return s->str[stack_size(s) - 1];
}

//поместить значение x на верхушку стекаv
void stack_push(struct Stack* s, StackElement x) {
	if (s == NULL) {
		return;
	}
	int size = stack_size(s);
	if (size == SIZE - 1) { // стек полон
		return;
	}
	s->str[size] = x;
	s->str[size + 1] = '\0';
}

//достать значение с верхушки стека
StackElement stack_pop(struct Stack* s) {
	if (s == NULL) {
		return -1;
	}
	int size = stack_size(s);
	StackElement element = stack_peek(s);
	s->str[size - 1] = '\0';
	return element;
}

void stack_delete_element(struct Stack* s) {
	if (s == NULL) {
		return;
	}
	int size = stack_size(s);
	s->str[size - 1] = '\0';
}

int priority(char element) {
	if (element == '+' || element == '-') {
		return 1;
	}
	else if (element == '*' || element == '/') {
		return 2;
	}
	return 0;
}

bool stack_is_empty(struct Stack* s) {
	return stack_size(s) == 0;
}

bool operator (char element) {
	return priority(element) > 0;
}
bool is_number(char element) {
	return element >= '0' && element <= '9' || element == '$';
}
bool left_bracket(char element) {
	return element == '(';
}
bool right_bracket(char element) {
	return element == ')';
}
bool is_bracket(char element) {
	return left_bracket(element) || right_bracket(element);
}
bool is_end_symbol(char element) {
	return element == '\n';
}
bool is_dot(char element) {
	return element == '.';
}


bool check_input(char element, char next_element, int i, bool* dot_marker, char* type) {
	if (type == "int" && (element == '.' || element == '$')) {
		return false;
	}
	if (i == 0 && (operator(element) || right_bracket(element) || is_dot(element))) {
		return false;
	}
	if (element == '$' && is_number(next_element)) {
		return false;
	}
	if (is_number(element) && next_element == '$') {
		return false;
	}
	if (is_number(element) && left_bracket(next_element)) {
		return false;
	}
	if (operator(element) && (right_bracket(next_element) || operator(next_element))) {
		return false;
	}
	if (left_bracket(element) && (right_bracket(next_element) || operator(next_element))) {
		return false;
	}
	if (right_bracket(element) && (left_bracket(next_element) || is_number(next_element))) {
		return false;
	}
	if (!operator(element) && !is_bracket(element) && !is_number(element) && !is_dot(element)) {
		return false;
	}
	if ((operator(element) || left_bracket(element)) && is_end_symbol(next_element)) {
		return false;
	}
	if ((!is_number(element) || element == '$') && is_dot(next_element)) {
		return false;
	}
	if (is_dot(element)) {
		if (*dot_marker) {
			return false;
		}
		*dot_marker = true;
		if (!is_number(next_element)) {
			return false;
		}
		if (next_element == '$') {
			return false;
		}
	}
	return true;
}


bool is_bracket_in_stack(struct Stack* s) {
	if (s == NULL)
		return false;
	int size = stack_size(s);
	for (int i = size - 1; i >= 0; i--) {
		if (s->str[i] == '(') {
			return true;
		}
	}
	return false;
}

struct result calculate(char operator, double x, double y) {
	struct result temp = {
		.data = 0,
		.error = NULL,
	};
	switch (operator) {
	case '*':
		temp.data = x * y;
		return temp;
	case '+':
		temp.data = x + y;
		return temp;
	case '-':
		temp.data = x - y;
		return temp;
	}
	if (operator == '/') {
		if (y != 0) {
			temp.data = x / y;
			return temp;
		}
		else {
			temp.error = "division by zero\n";
			return temp;
		}
	}
	return temp;
}


char* slice(const char* string, int start, int len) {
	char* sliced_string = (char*)malloc(len * sizeof(char));
	for (int i = 0; i < len; i++) {
		sliced_string[i] = string[i + start];
	}
	return sliced_string;
}


char* to_reverse_polish(char* input, char* type) {
	struct Stack stack;

	int len = strlen(input);
	int count = 0;
	char* result = malloc(SIZE * sizeof(char));
	char* error = malloc(SIZE * sizeof(char));
	strcpy(error, "syntax error");
	stack_init(&stack);
	bool is_dot_in_number = false;


	if (len == 1) {
		free(result);
		return error;
	}
	for (int i = 0; i < len - 1; i++) {
		if (!check_input(input[i], input[i + 1], i, &is_dot_in_number, type)) {
			free(result);
			return error;
		}
		if (is_number(input[i]) || is_dot(input[i])) {
			result[count] = input[i];
			count++;
			if (!is_number(input[i + 1]) && !is_dot(input[i + 1])) {
				result[count] = ' ';
				count++;
			}
		}
		else {
			is_dot_in_number = false;
			if (!stack_is_empty(&stack)) {
				if (input[i] == '(') {
					stack_push(&stack, input[i]);
					continue;
				}
				if (input[i] == ')') {
					if (!is_bracket_in_stack(&stack)) {
						free(result);
						return error;
					}
					while (stack_peek(&stack) != '(') {
						char element = stack_pop(&stack);
						result[count] = element;
						count++;
					}
					if (stack_peek(&stack) == '(') {
						stack_delete_element(&stack);
						continue;
					}
				}
				int priority_input = priority(input[i]);
				int priority_stack = priority(stack_peek(&stack));
				if (priority_stack >= priority_input) {
					char element = stack_pop(&stack);
					result[count] = element;
					count++;
				}
				stack_push(&stack, input[i]);
			}
			else {
				stack_push(&stack, input[i]);
			}
		}
	}
	if (is_bracket_in_stack(&stack)) {
		free(result);
		return error;
	}
	while (!stack_is_empty(&stack)) {
		result[count] = stack_pop(&stack);
		count++;
	}
	result[count] = '\0';
	free(error);
	return result;
}


struct result calculate_polish(char* input, double previous_result) {
	int len = strlen(input);
	double stack[SIZE] = { 0 };
	int size_of_stack = 0;
	struct result output = {
		.data = 0,
		.error = NULL,
	};
	for (int i = 0; i < len; i++) {
		if (is_number(input[i])) {
			double intermediate_result = 0;
			if (input[i] != '$') {
				double counter = 0;
				while (is_number(input[i]) || is_dot(input[i])) {
					counter++;
					i++;
				}
				char* slced_string = slice(input, i - counter, counter);
				intermediate_result = atof(slced_string);
				free(slced_string);
			}
			else {
				intermediate_result = previous_result;
			}
			output.data = intermediate_result;
			stack[size_of_stack] = intermediate_result;
			size_of_stack++;
		}
		if (operator(input[i])) {
			double x = stack[size_of_stack - 1];
			double y = stack[size_of_stack - 2];
			size_of_stack--;
			output = calculate(input[i], y, x);
			if (!output.error) {
				stack[size_of_stack - 1] = output.data;
			}
			else {
				return output;
			}
		}
	}
	return output;

}


int main(int argc, char* argv[]) {
	/*char string[SIZE + 1];
	bool previous_result_exist = false;
	double previous_result = 0;
	if (!fgets(string, SIZE + 1, stdin)) {
		return 0;
	}
	char* output = to_reverse_polish(string);
	if (!output || strcmp(output, "syntax error") == 0) {
		printf("syntax error");
	}
	else {
		struct result res = calculate_polish(output, previous_result_exist, previous_result);
		if (!res.error) {
			printf("%lf", res.data);
		}
		else {
			printf("%s", res.error);
		}
	}
	free(output);*/

	double previous_result = 0;

	if (argc == 2 && strcmp(argv[1], "-i") == 0) {
		char string[SIZE + 1];

		bool exit_flag = true;
		printf("Enter interactive mode\n");
		while (exit_flag) {
			printf("in > ");
			if (!fgets(string, SIZE + 1, stdin)) {
				return 0;
			}
			if (strcmp(string, "exit\n") == 0) {
				exit_flag = false;
				printf("Exit from interactive mode");
				break;
			}
			else {
				char* output = to_reverse_polish(string, "double");
				if (!output || strcmp(output, "syntax error") == 0) {
					printf("syntax error");
					free(output);
					continue;
				}
				else {
					struct result res = calculate_polish(output, previous_result);
					if (!res.error) {
						previous_result = res.data;
						printf("out < %.2lf\n", res.data);
						free(output);
					}
					else {
						printf("%s", res.error);
						free(output);
					}
				}
			}
		}
	}
	else {
		char string[SIZE + 1];
		if (!fgets(string, SIZE + 1, stdin)) {
			return 0;
		}
		char* output = to_reverse_polish(string, "int");
		if (!output || strcmp(output, "syntax error") == 0) {
			printf("syntax error");
		}
		else {
			struct result res = calculate_polish(output, previous_result);
			if (!res.error) {
				printf("%lf", res.data);
			}
			else {
				printf("%s", res.error);
			}
		}
		free(output);
	}
	return 0;
}
