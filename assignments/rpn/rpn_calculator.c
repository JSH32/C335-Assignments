#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Stack data structure with float data type.
 */
typedef struct Stack {
  int size;
  int top;
  long *buf;
} Stack;

/**
 * @brief Create a new stack with the given initial size.
 *
 * @param initialSize Initial size of the stack
 * @return Stack
 */
Stack new_stack(int initialSize) {
  Stack stack = {.size = initialSize,
                 .buf = malloc(initialSize * sizeof(long))};

  return stack;
}

/**
 * @brief Push a value to the top of the stack.
 *
 * @param stack stack
 * @param value value to push
 */
void stack_push(Stack *stack, float value) {
  if (stack->top == stack->size) {
    stack->size *= 2;
    stack->buf = realloc(stack->buf, stack->size * sizeof(long));
  }

  stack->buf[stack->top++] = value;
}

/**
 * @brief Pop a value from the top of the stack.
 *
 * @param stack stack
 * @return popped value
 */
long stack_pop(Stack *stack) { return stack->buf[--stack->top]; }

/**
 * @brief Free the stack's buffer
 *
 * @param stack  stack
 */
void stack_free(Stack *stack) { free(stack->buf); }

int main(int argc, char *argv[]) {
  Stack stack = new_stack(10);

  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];

    if (!strcmp(arg, "add")) {
      float a = stack_pop(&stack);
      float b = stack_pop(&stack);
      stack_push(&stack, a + b);
    } else if (!strcmp(arg, "sub")) {
      float a = stack_pop(&stack);
      float b = stack_pop(&stack);
      stack_push(&stack, b - a);
    } else if (!strcmp(arg, "mult")) {
      float a = stack_pop(&stack);
      float b = stack_pop(&stack);
      stack_push(&stack, b * a);
    } else if (!strcmp(arg, "div")) {
      float a = stack_pop(&stack);
      float b = stack_pop(&stack);
      stack_push(&stack, b / a);
    } else {
      // Push to stack if not an operator.
      long value = strtol(arg, NULL, 10);
      stack_push(&stack, value);
    }
  }

  printf("%ld\n", stack_pop(&stack));
  stack_free(&stack);

  return 0;
}