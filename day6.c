#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_file(FILE *f);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    parse_file(f);

    if (ferror(f)) {
        perror("read error");
    }

    fclose(f);
    return 0;
}

void parse_file(FILE *f) {
    char lines[5][4000];
    char operators[1000];
    size_t i = 0;
    while (i < 5 && fgets(lines[i], sizeof(lines[i]), f)) {
        i++;
    }
    size_t rows = i;

    size_t operator_count = 0;
    int offset = 0, n;
    char next_op[100];
    while (sscanf(lines[rows - 1] + offset, " %[+*]%n", next_op, &n) == 1) {
        operators[operator_count] = next_op[0];
        operator_count++;
        offset += n;
    }

    size_t operand_count;
    u_int64_t operands[5][1000];
    u_int64_t next_opr;
    for (size_t r = 0; r < rows - 1; r++) {
        offset = 0;
        operand_count = 0;
        while (sscanf(lines[r] + offset, " %llu%n", &next_opr, &n) == 1) {
            operands[r][operand_count] = next_opr;
            operand_count++;
            offset += n;
        }
    }

    u_int64_t sum = 0;
    for (size_t i = 0; i < operator_count; i++) {
        u_int64_t result = operands[0][i];
        for (size_t r = 1; r < rows - 1; r++) {
            if (operators[i] == '+') {
                result += operands[r][i];
            } else if (operators[i] == '*') {
                result *= operands[r][i];
            }
        }
        sum += result;
    }
    printf("%llu\n", sum);
}
