#include "matrix.h"
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int indicator_lights[10];
    int joltage_levels[10];
    int buttons[15][10];
    int no_buttons;
} Machine;

typedef struct {
    Vector **combinations;
    size_t *comb_index;
    size_t num_free_vars;
    int *max_presses;
} CombsData;

bool parse_file(const char *filename);
void print_machine(const Machine *m);
int solve(Machine *m, bool joltage);
Matrix *setup_eqs(Machine *m, bool joltage);
void row_echelon_form(Matrix *m, size_t max_cols);
void get_pivots(const Matrix *m, size_t *pivots, size_t *num_pivots,
                size_t *free_col, size_t *num_free_col);
Matrix *coeff_matrix(Matrix *m, size_t *free_vars, size_t num_free_vars,
                     size_t num_pivots);
int get_max_possible_presses(Machine *m, size_t button_index);
Vector **get_free_variable_vectors(Machine *m, size_t *free_vars,
                                   size_t num_free_vars, size_t *out_size);
void get_combs(CombsData *data, int *stack, size_t height);
int is_near_integer(double value);
int is_non_negative_within_epsilon(double value);
bool check_solution(Vector *sol);

Machine machines[200];
int no_machines = 0;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    if (!parse_file(argv[1])) {
        return 1;
    }

    // for (int i = 0; i < no_machines; i++) {
    //     print_machine(&machines[i]);
    // }


    // printf("Solving machine %d for joltage levels:\n", 4);
    // int presses = solve(&machines[4], true);
    // printf("Minimum button presses: %d\n", presses);
    
    int sum1 = 0;
    int sum2 = 0;
    int presses;
    for (int i = 0; i < no_machines; i++) {
        // printf("Solving machine %d for indicator lights:\n", i);
        // solve(&machines[i], false);
        // printf("Solving machine %d for joltage levels:\n", i);
        presses = solve(&machines[i], true);
        sum2 += presses;
        // printf("Minimum button presses: %d\n", presses);
    }
    printf("Sum of minimum button presses for joltage levels: %d\n", sum2);

    return 0;
}

// void tester(int** combinations, int* comb_index, int *values, int depth) {
//     if (depth == 3) {
//         combinations[*comb_index] = malloc(3 * sizeof(int));
//         for (int i = 0; i < 3; i++) {
//             combinations[*comb_index][i] = values[i];
//         }
//         (*comb_index)++;
//         return;
//     }
//
//     for (int i = 0; i < 5; i++) {
//         values[depth] = i;
//         tester(combinations, comb_index, values, depth + 1);
//     }
// }

int solve(Machine *m, bool joltage) {
    Matrix *eq_matrix = setup_eqs(m, joltage);

    // printf("\n");
    // matrix_print(eq_matrix);

    row_echelon_form(eq_matrix, eq_matrix->cols - 1);

    // printf("\n");
    // matrix_print(eq_matrix);

    size_t pivots[10];
    size_t num_pivots;
    size_t free_vars[10];
    size_t num_free_vars = 0;
    get_pivots(eq_matrix, pivots, &num_pivots, free_vars, &num_free_vars);

    // printf("Pivots at columns: ");
    // for (size_t i = 0; i < num_pivots; i++) {
    //     printf("%zu ", pivots[i]);
    // }
    // printf("\n\n");
    //
    // printf("Free variables at columns: ");
    // for (size_t i = 0; i < num_free_vars; i++) {
    //     printf("%zu ", free_vars[i]);
    // }
    // printf("\n\n");

    Matrix *coeffs =
        coeff_matrix(eq_matrix, free_vars, num_free_vars, num_pivots);

    // printf("Coefficient matrix:\n");
    // matrix_print(coeffs);

    size_t num_vectors;
    Vector **free_var_vectors =
        get_free_variable_vectors(m, free_vars, num_free_vars, &num_vectors);

    int min_presses = INT_MAX;
    for (size_t i = 0; i < num_vectors; i++) {
        Vector *solution = matrix_multiply_vector(coeffs, free_var_vectors[i]);

        // printf("Solution %zu:\n", i + 1);
        // vector_print(solution);

        if (check_solution(solution)) {
            int sum = 0;
            for (size_t j = 0; j < solution->size; j++) {
                double val = vector_get(solution, j);
                sum += (int)round(val);
            }
            if (sum < min_presses) {
                min_presses = sum;
            }
        }
        vector_free(solution);
    }

    for (size_t i = 0; i < num_vectors; i++) {
        vector_free(free_var_vectors[i]);
    }
    free(free_var_vectors);

    matrix_free(coeffs);
    matrix_free(eq_matrix);

    return min_presses;
}

bool check_solution(Vector *sol) {
    for (size_t i = 0; i < sol->size; i++) {
        double val = vector_get(sol, i);
        if (!is_near_integer(val) || !is_non_negative_within_epsilon(val)) {
            return false;
        }
    }
    return true;
}

int is_near_integer(double value) {
    double rounded = round(value);
    return fabs(value - rounded) <= 1e-6;
}

int is_non_negative_within_epsilon(double value) { return value >= -1e-6; }

Vector **get_free_variable_vectors(Machine *m, size_t *free_vars,
                                   size_t num_free_vars, size_t *out_size) {
    int max_presses[3] = {0};
    for (size_t i = 0; i < num_free_vars; i++) {
        size_t button_index = free_vars[i];
        max_presses[i] = get_max_possible_presses(m, button_index);
    }
    size_t total_combinations = 1;
    for (size_t i = 0; i < num_free_vars; i++) {
        total_combinations *= (max_presses[i] + 1);
    }
    Vector **vectors = malloc(total_combinations * sizeof(Vector *));
    for (size_t i = 0; i < total_combinations; i++) {
        vectors[i] = vector_create(num_free_vars + 1);
    }
    *out_size = 0;
    CombsData data = {
        vectors,
        out_size,
        num_free_vars,
        max_presses,
    };
    int stack[3] = {0};
    int stack_height = 0;
    get_combs(&data, stack, stack_height);

    // for (size_t i = 0; i < comb_index; i++) {
    //     vector_print(vectors[i]);
    // }

    return vectors;
}

void get_combs(CombsData *data, int *stack, size_t height) {
    size_t num_free_vars = data->num_free_vars;
    size_t *comb_index = data->comb_index;
    Vector **combinations = data->combinations;
    int *max_presses = data->max_presses;

    if (height == num_free_vars) {
        for (size_t i = 0; i < height; i++) {
            vector_set(combinations[*comb_index], i, stack[i]);
        }
        vector_set(combinations[*comb_index], height, 1.0);
        (*comb_index)++;
        return;
    }

    for (int i = 0; i <= max_presses[height]; i++) {
        stack[height] = i;
        get_combs(data, stack, height + 1);
    }
}

int get_max_possible_presses(Machine *m, size_t button_index) {
    int max_presses = INT_MAX;
    for (int i = 0; i < 10; i++) {
        if (m->buttons[button_index][i] != 0) {
            int possible_presses = m->joltage_levels[i];
            if (possible_presses < max_presses) {
                max_presses = possible_presses;
            }
        }
    }
    return max_presses;
}

Matrix *coeff_matrix(Matrix *m, size_t *free_vars, size_t num_free_vars,
                     size_t num_pivots) {
    Matrix *coeffs =
        matrix_create(num_free_vars + num_pivots, num_free_vars + 1);

    for (size_t i = 0; i < num_free_vars; i++) {
        matrix_set(coeffs, i, i, 1.0);
    }

    for (size_t i = 0; i < num_pivots; i++) {
        for (size_t j = 0; j < num_free_vars; j++) {
            size_t free_var_col = free_vars[j];
            double val = -matrix_get(m, i, free_var_col);
            matrix_set(coeffs, num_free_vars + i, j, val);
        }
        double const_term = matrix_get(m, i, m->cols - 1);
        matrix_set(coeffs, num_free_vars + i, num_free_vars, const_term);
    }
    return coeffs;
}

void get_pivots(const Matrix *m, size_t *pivots, size_t *num_pivots,
                size_t *free_col, size_t *num_free_col) {
    bool *is_pivot_column = calloc(m->cols, sizeof(bool));
    for (size_t r = 0; r < m->rows; r++) {
        for (size_t c = 0; c < m->cols; c++) {
            if (matrix_get(m, r, c) != 0) {
                is_pivot_column[c] = true;
                break;
            }
        }
    }

    *num_pivots = 0;
    *num_free_col = 0;
    for (size_t c = 0; c < m->cols - 1; c++) {
        if (is_pivot_column[c]) {
            pivots[(*num_pivots)++] = c;
        } else {
            free_col[(*num_free_col)++] = c;
        }
    }

    free(is_pivot_column);
}

void row_echelon_form(Matrix *m, size_t max_cols) {
    size_t lead = 0;
    for (size_t r = 0; r < m->rows; r++) {
        if (lead >= max_cols) {
            return;
        }
        size_t i = r;
        while (matrix_get(m, i, lead) == 0) {
            i++;
            if (i == m->rows) {
                i = r;
                lead++;
                if (lead >= max_cols) {
                    return;
                }
            }
        }
        for (size_t j = 0; j < m->cols; j++) {
            double temp = matrix_get(m, r, j);
            matrix_set(m, r, j, matrix_get(m, i, j));
            matrix_set(m, i, j, temp);
        }
        double div = matrix_get(m, r, lead);
        for (size_t j = 0; j < m->cols; j++) {
            matrix_set(m, r, j, matrix_get(m, r, j) / div);
        }
        for (size_t i = 0; i < m->rows; i++) {
            if (i != r) {
                double sub = matrix_get(m, i, lead);
                for (size_t j = 0; j < m->cols; j++) {
                    matrix_set(m, i, j,
                               matrix_get(m, i, j) - sub * matrix_get(m, r, j));
                }
            }
        }
        lead++;
    }
}

Matrix *setup_eqs(Machine *m, bool joltage) {
    Matrix *eq_matrix = matrix_create(10, m->no_buttons + 1);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < m->no_buttons; j++) {
            matrix_set(eq_matrix, i, j, m->buttons[j][i]);
        }
        matrix_set(eq_matrix, i, m->no_buttons,
                   joltage ? m->joltage_levels[i] : m->indicator_lights[i]);
    }
    return eq_matrix;
}

void parse_bool_list(const char *s, int state[10]) {
    for (size_t i = 0; i < 10 && s[i] != '\0'; i++) {
        if (s[i] == '#') {
            state[i] = 1;
        } else {
            state[i] = 0;
        }
    }
}

void parse_position_list(const char *s, int positions[10]) {
    memset(positions, 0, 10 * sizeof(int));
    char *token;
    char *str = strdup(s);
    token = strtok(str, ",");
    while (token != NULL) {
        int pos = atoi(token);
        if (pos >= 0 && pos < 10) {
            positions[pos] = 1;
        }
        token = strtok(NULL, ",");
    }
    free(str);
}

void parse_number_list(const char *s, int button[10]) {
    char *token;
    char *str = strdup(s);
    int i = 0;
    token = strtok(str, ",");
    while (token != NULL && i < 10) {
        button[i] = atoi(token);
        token = strtok(NULL, ",");
        i++;
    }
    free(str);
}

void parse_machine(char *line, Machine *m) {
    char item[64];
    int n;
    sscanf(line, "[%[.#]]%n", item, &n);
    parse_bool_list(item, m->indicator_lights);
    line += n;

    while (sscanf(line, " (%[0-9,])%n", item, &n)) {
        parse_position_list(item, m->buttons[m->no_buttons]);
        m->no_buttons++;
        line += n;
    }

    sscanf(line, " {%[0-9,]}", item);
    parse_number_list(item, m->joltage_levels);
}

void print_machine(const Machine *m) {
    printf("Indicator lights:\n");
    for (int i = 0; i < 10; i++) {
        printf("%d,", m->indicator_lights[i]);
    }
    printf("\nButtons:\n");
    for (int i = 0; i < m->no_buttons; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%d,", m->buttons[i][j]);
        }
        printf("\n");
    }
    printf("Joltage levels:\n");
    for (int i = 0; i < 10; i++) {
        printf("%d,", m->joltage_levels[i]);
    }
    printf("\n\n");
}

void parse_input(FILE *f) {
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        parse_machine(line, &machines[no_machines]);
        no_machines++;
    }
    printf("Parsed %d machines\n", no_machines);
}

bool parse_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return false;
    }

    parse_input(f);

    if (ferror(f)) {
        perror("read error");
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}
