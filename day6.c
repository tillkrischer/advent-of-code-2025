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

bool is_empty_col(char lines[5][4000], size_t rows, size_t col) {
    for (size_t r = 0; r < rows; r++) {
        if (lines[r][col] != ' ') {
            return false;
        }
    }
    return true;
}

size_t get_operands_by_rows(char lines[5][4000], size_t rows, size_t start_col,
                            size_t end_col, u_int64_t operands[5]) {
    for (size_t r = 0; r < rows - 1; r++) {
        u_int64_t operand = 0;
        for (size_t c = start_col; c < end_col; c++) {
            if (lines[r][c] >= '0' && lines[r][c] <= '9') {
                operand = operand * 10 + (lines[r][c] - '0');
            }
        }
        operands[r] = operand;
    }
    return rows - 1;
}

size_t get_operands_by_cols(char lines[5][4000], size_t rows, size_t start_col,
                            size_t end_col, u_int64_t operands[5]) {
    for (size_t c = start_col; c < end_col; c++) {
        u_int64_t operand = 0;
        for (size_t r = 0; r < rows - 1; r++) {
            if (lines[r][c] >= '0' && lines[r][c] <= '9') {
                operand = operand * 10 + (lines[r][c] - '0');
            }
        }
        operands[c - start_col] = operand;
    }
    return end_col - start_col;
}

u_int64_t solve_problem(char lines[5][4000], size_t rows, size_t start_col,
                        size_t end_col, bool by_cols) {
    u_int64_t operands[5];
    size_t operand_count;
    if (by_cols) {
        operand_count =
            get_operands_by_cols(lines, rows, start_col, end_col, operands);
    } else {
        operand_count =
            get_operands_by_rows(lines, rows, start_col, end_col, operands);
    }
    u_int64_t result = operands[0];
    char op = lines[rows - 1][start_col];
    for (size_t i = 1; i < operand_count; i++) {
        if (op == '+') {
            result += operands[i];
        } else if (op == '*') {
            result *= operands[i];
        }
    }
    return result;
}

void parse_file(FILE *f) {
    char lines[5][4000];
    size_t i = 0;
    while (i < 5 && fgets(lines[i], sizeof(lines[i]), f)) {
        i++;
    }
    size_t rows = i;
    size_t cols = strlen(lines[0]);

    int problem_start_cols[1000];
    problem_start_cols[0] = 0;
    size_t start_col_count = 1;
    for (size_t i = 0; i < cols; i++) {
        if (is_empty_col(lines, rows, i)) {
            problem_start_cols[start_col_count++] = i + 1;
        }
    }

    u_int64_t sum1 = 0, sum2 = 0;
    for (size_t i = 0; i < start_col_count; i++) {
        size_t start_col = problem_start_cols[i];
        size_t end_col = (i + 1 < start_col_count)
                             ? problem_start_cols[i + 1] - 1
                             : cols - 1;

        sum1 += solve_problem(lines, rows, start_col, end_col, false);
        sum2 += solve_problem(lines, rows, start_col, end_col, true);
    }
    printf("%llu\n", sum1);
    printf("%llu\n", sum2);
}
