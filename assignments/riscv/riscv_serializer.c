#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NOT_IMPLEMENTED -1

/*
 *  Stduent functions - Full in each so that they return the integer value of the requested instruction
 */
int generate_load_instruction(int width, int destination, int address, int offset) {
  int instruction = 0;

  // Set opcode, 0-6 stored in 7 bits.
  instruction |= (0x3) | ((0x0 & 0x1F) << 2);
  // Set destination, 7-11 stored in 5 bits.
  instruction |= (destination & 0x1F) << 7;
  // Set width, 12-14 stored in 3 bits.
  instruction |= (width & 0x7) << 12;
  // Set source register address, 15-19 stored in 5 bits.
  instruction |= (address & 0x1F) << 15;
  // Set immediate offset, 20-31 stored in 12 bits.
  instruction |= (offset & 0xFFF) << 20;

  return instruction;
}

int generate_store_instruction(int width, int address, int source, int offset) {
  int instruction = 0;

  // Set opcode, 0-6 stored in 7 bits
  instruction |= (0x3) | ((0x8 & 0x1F) << 2);
  // Set lower immediate bits, 7-11
  instruction |= ((offset & 0x1F) & 0x1F) << 7;
  // Set funct3 (width), 12-14 stored in 3 bits (width indicates the size of the data being stored)
  instruction |= (width & 0x7) << 12;
  // Set base register (address), 15-19 stored in 5 bits
  instruction |= (address & 0x1F) << 15;
  // Set source register (source), 20-24 stored in 5 bits
  instruction |= (source & 0x1F) << 20;
  // Set upper immediate bits, 25-31
  instruction |= (((offset >> 5) & 0x7F) & 0x7F) << 25;

  return instruction;
}

int generate_math_instruction(int function, int a, int b, int destination) {
  int instruction = 0;

  // Set opcode, 0-6 stored in 7 bits.
  instruction |= (0x3) | ((0xC & 0x1F) << 2);
  // Set destination register, 7-11 stored in 5 bits.
  instruction |= (destination & 0x1F) << 7;
  // Set function (funct3), 12-14 stored in 3 bits.
  instruction |= (function & 0x7) << 12;
  // Set first operand, 15-19 stored in 5 bits.
  instruction |= (a & 0x1F) << 15;
  // Set second operand, 20-24 stored in 5 bits.
  instruction |= (b & 0x1F) << 20;

  return instruction;
}

int generate_constant_instruction(int value, int destination) {
  int instruction = 0;

  // Set opcode, 0-6 stored in 7 bits.
  instruction |= (0x3) | ((0xD & 0x1F) << 2);
  // Set destination register, 7-11 stored in 5 bits.
  instruction |= (destination & 0x1F) << 7;
  // Set constant value, 12-31 stored in 20 bits.
  instruction |= (value & 0xFFFFF) << 12;

  return instruction;
}

/*                                                                                    *
 *  Helper functions used by main to convert string instructions into numeric values  *
 *                                                                                    */
/*  Calculate the integer that represents the instruction bit width based on its text  */
int get_width(const char* str) {
  switch (str[1]) {
    case 'b': {
      if (str[2] == 'u')
        return 0x4;
      return 0x0;
    }
    case 'h': {
      if (str[2] == 'u')
        return 0x5;
      return 0x1;
    }
    case 'w':
      return 0x2;
    default:
      printf("[ERROR] Unknown width index\n");
      exit(-2);
  }
}

/*  Calculate the integer index that represents the instruction function based on its text  */
int get_func(const char* str) {
  const char* fns[] = { "add", "sll", "slt", "sltu", "xor", "srl", "or", "and" };
  for (int i = 0; i < 8; i++)
    if (strcmp(fns[i], str) == 0)
      return i;
  printf("[ERROR] Unknown function name\n");
  exit(-5);
}

/*  Calculates the integer register index based on the register name  */
static const char* regs[] = { "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6" };
int get_reg(const char* str) {
  int i = 0;
  for (; str[i] != '\0'; i++)
    if (str[i] == ',')
      break;

  for (int j = 0; j < 32; j++) {
    if (strncmp(str, regs[j], i) == 0)
      return j;
  }
  printf("[ERROR] Unknown register index\n");
  exit(-3);
}

/*  Parses the integer register index and integer offset of a register from a memory style instruction i.e. 8(a0)  */
int get_mem_reg(const char* str, int* offset) {
  int i = 0;
  char* ptr;
  *offset = strtol(str, &ptr, 0);
  if (str == ptr) {
    printf("[ERROR] Bad memory address syntax\n");
    exit(-4);
  }

  for (i = 0; ptr[i] != ')' && ptr[i] != '\0'; i++)
    ;
  if (ptr[0] != '(' || ptr[i] != ')') {
    printf("[ERROR] Bad memory address syntax\n");
    exit(-4);
  }

  ++ptr;
  for (int j = 0; j < 32; j++)
    if (strncmp(ptr, regs[j], i - 1) == 0)
      return j;
  printf("[ERROR] Unknown register index\n");
  exit(-3);
}

/*  Check if a function has been implemented  */
int is_implemented(const char* name, int (*fn)(int, int, int, int), int width, int destination, int source, int offset) {
  int opcode = fn(width, destination, source, offset);
  if (opcode == NOT_IMPLEMENTED) {
    printf("[ERROR] `%s` has not been implemented\n", name);
    exit(-2);
  }
  return opcode;
}
int is_constant_implemented(const char* name, int (*fn)(int, int), int value, int destination) {
  int opcode = fn(value, destination);
  if (opcode == NOT_IMPLEMENTED) {
    printf("[ERROR] `%s` has not been implemented\n", name);
    exit(-2);
  }
  return opcode;
}

/*
 *  Main function converts an assembly instruction into an integer and prints the result
 */
int main(int argc, char** argv) {
  int j = 1;
  char* args[5];
  args[j++] = argv[1];
  if (argc != 2) {
    printf("[ERROR] Too many arguments\n");
    return -1;
  }

  for (int i = 0; argv[1][i] != '\0'; i++) {
    if (argv[1][i] == ' ') {
      argv[1][i] = '\0';
      args[j++]  = &(argv[1][++i]);
    }
  }

  if (strcmp(args[1], "lui") == 0) {
    if (j != 4) {
      printf("[ERROR] Incorrect arguments for constant\n");
      return -2;
    }

    int value = strtol(args[3], NULL, 0), dst = get_reg(args[2]);
    printf("Breakdown:\n  [type=constant] [value=0x%x] [dst=0x%x]\n", value, dst);
    printf("\n  Instruction: 0x%x\n\n", is_constant_implemented("generate_constant_instruction", generate_constant_instruction, value, dst));
    return 0;
  } else if (args[1][0] == 's') {
    if (j != 4) {
      printf("[ERROR] Incorrect arguments for store\n");
      return -2;
    }
    int offset, width = get_width(args[1]);
    int reg = get_mem_reg(args[3], &offset);
    printf("Breakdown:\n  [type=store] [width=0x%x] [addr=0x%x] [src=0x%x] [offset=0x%x]\n", width, reg, get_reg(args[2]), offset);
    printf("\n  Instruction: 0x%x\n\n", is_implemented("generate_store_instruction", generate_store_instruction, width, reg, get_reg(args[2]), offset));
  } else if (args[1][0] == 'l') {
    if (j != 4) {
      printf("[ERROR] Incorrect arguments for load\n");
      return -2;
    }
    int width = get_width(args[1]);
    int offset;
    int reg = get_mem_reg(args[3], &offset);
    printf("Breakdown:\n  [type=load] [width=0x%x] [dst=0x%x] [addr=0x%x] [offset=0x%x]\n", width, get_reg(args[2]), reg, offset);
    printf("\n  Instruction: 0x%x\n\n", is_implemented("generate_load_instruction", generate_load_instruction, width, get_reg(args[2]), reg, offset));
  } else {
    printf("Breakdown:\n  [type=math] [func=0x%x] [a=0x%x] [b=0x%x] [dst=0x%x]\n", get_func(args[1]), get_reg(args[3]), get_reg(args[4]), get_reg(args[2]));
    printf("\n  Instruction: 0x%x\n\n", is_implemented("generate_math_instruction", generate_math_instruction, get_func(args[1]), get_reg(args[3]), get_reg(args[4]), get_reg(args[2])));
  }
}
