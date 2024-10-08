#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_num(float num) {
    unsigned int *ptr = (unsigned int*)&num;
    unsigned int sign = (*ptr >> 31) & 1;
    unsigned int exponent = (*ptr >> 23) & 0xFF;
    unsigned int mantissa = (*ptr & 0x7FFFFF);

    printf("Binary: ");
    for (int i = 31; i >= 0; i--) {
        printf("%d", (*ptr >> i) & 1);
    }
    printf("\n");

    printf("Sign: %d\n", sign);
    if (!exponent) {
        printf("Exponent: -126\n");
        printf("Mantissa: %.7g\n", (float)mantissa / (1 << 23));
    } else {
        printf("Exponent: %d\n", exponent - 127);
        printf("Mantissa: %.7g\n", 1.0f + (float)mantissa / (1 << 23));
    }
    printf("Value: %.7g\n", num);
}

void f_mode(char *arg) {
    float f_arg = strtof(arg, NULL);
    print_num(f_arg);
}

void b_mode(char *arg) {
    if (strlen(arg) != 32) {
        printf("Invalid input\n");
        return;
    }

    unsigned int int_val = 0;
    for (int i = 0; i < 32; i++) {
        int_val <<= 1;
        int_val |= (arg[i] == '1') ? 1 : 0;
    }

    float f = *((float*)&int_val);
    print_num(f);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s -f/-b <filename>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-f") == 0) {
        f_mode(argv[2]);
    } else if (strcmp(argv[1], "-b") == 0) {
        b_mode(argv[2]);
    } else {
        printf("Usage: %s -f/-b <filename>\n", argv[0]);
        return 1;
    }

    return 0;
}
