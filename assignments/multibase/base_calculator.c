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
  unsigned int* buf;
} Stack;

/**
 * @brief Create a new stack with the given initial size.
 *
 * @param initialSize Initial size of the stack
 * @return Stack
 */
Stack stack_new(int initialSize) {
  Stack stack = {
    .size = initialSize,
    .buf  = malloc(initialSize * sizeof(unsigned int))
  };

  return stack;
}

/**
 * @brief Push a value to the top of the stack.
 *
 * @param stack stack
 * @param value value to push
 */
void stack_push(Stack* stack, unsigned int value) {
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
unsigned int stack_pop(Stack* stack) {
  return stack->buf[--stack->top];
}

/**
 * @brief Free the stack's buffer
 *
 * @param stack  stack
 */
void stack_free(Stack* stack) {
  free(stack->buf);
}

/**
 * @brief Convert hex char to int
 *
 * @param c char
 * @return int
 */
int hex_to_int(char c) {
  if (c >= '0' && c <= '9') {
    // Convert '0'-'9' to 0-9
    return c - '0';
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  } else {
    return -1;
  }
}

/**
 * @brief Parse an argument into an unsigned int with base2, base10, base16
 *
 * @param arg string of number
 * @return unsigned int
 */
unsigned int parse_arg(char* arg) {
  int len            = strlen(arg);
  unsigned int value = 0;

  if (len > 2 && arg[0] == '0' && arg[1] == 'b') {
    // Base2
    char* ptr = arg + (sizeof(char) * 2);
    int idx   = len - 3; // index should be an index, 0 included.

    while (*ptr) {
      if (*ptr == '1') {
        value += (1 << idx); // Set the bit at position idx
      }
      ptr += sizeof(char);
      idx--;
    }
  } else if (len > 2 && arg[0] == '0' && arg[1] == 'x') {
    char* ptr = arg + (sizeof(char) * 2);
    int idx   = len - 3; // index should be an index, 0 included.

    while (*ptr) {
      // Add hex digit with proper place to value.
      // Shifting by 4 bits is basically multiplying by 16 to the power of idx.
      value += hex_to_int(*ptr) << (idx * 4);

      ptr += sizeof(char);
      idx--;
    }
  } else {
    // Base10
    while (*arg) {
      value *= 10;
      value += *arg - '0';
      arg++;
    }
  }

  return value;
}

int main(int argc, char* argv[]) {
  Stack stack = stack_new(10);

  for (int i = 1; i < argc; i++) {
    char* arg = argv[i];

    if (!strcmp(arg, "add")) {
      unsigned int a = stack_pop(&stack);
      unsigned int b = stack_pop(&stack);
      stack_push(&stack, a + b);
    } else if (!strcmp(arg, "sub")) {
      unsigned int a = stack_pop(&stack);
      unsigned int b = stack_pop(&stack);
      stack_push(&stack, b - a);
    } else if (!strcmp(arg, "mult")) {
      unsigned int a = stack_pop(&stack);
      unsigned int b = stack_pop(&stack);
      stack_push(&stack, b * a);
    } else if (!strcmp(arg, "div")) {
      unsigned int a = stack_pop(&stack);
      unsigned int b = stack_pop(&stack);
      stack_push(&stack, b / a);
    } else {
      // Push to stack if not an operator.
      unsigned int value = parse_arg(arg);
      stack_push(&stack, value);
    }
  }

  printf("%u\n", stack_pop(&stack));
  stack_free(&stack);

  return 0;
}
