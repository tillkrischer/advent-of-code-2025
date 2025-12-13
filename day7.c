#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool parse_file(const char *filename);
void parse_input(FILE *f);
void add_splitter(size_t row, size_t col);
u_int64_t walk(size_t row, size_t col);
u_int64_t count_visited();

typedef struct {
    u_int64_t no_splitters;
    u_int64_t splitters[50];
    bool visited[50];
} Row;

Row rows[150];
u_int64_t no_rows = 0;
u_int64_t no_cols = 0;
u_int64_t start_particle_pos = 0;

u_int64_t dp[150][150];

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    if (!parse_file(argv[1])) {
        return 1;
    }

    if (no_rows == 0) {
        fprintf(stderr, "Error: No data parsed\n");
        return 1;
    }

    u_int64_t paths = walk(0, start_particle_pos);
    u_int64_t visited = count_visited();

    printf("%llu\n", visited);
    printf("%llu\n", paths);

    return 0;
}

u_int64_t walk(size_t row, size_t col) {
    if (row >= no_rows) {
        return 1;
    }

    if (dp[row][col] != 0) {
        return dp[row][col];
    }

    Row *current_row = &rows[row];
    u_int64_t paths = 0;
    for (size_t i = 0; i < current_row->no_splitters; i++) {
        if (current_row->splitters[i] == col) {
            current_row->visited[i] = true;
            paths += walk(row + 1, col - 1);
            paths += walk(row + 1, col + 1);
            dp[row][col] = paths;
            return paths;
        }
    }

    paths = walk(row + 1, col);
    dp[row][col] = paths;
    return paths;
}

void parse_input(FILE *f) {
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        if (no_cols == 0) {
            no_cols = strlen(line) - 1;
        }
        for (size_t i = 0; i < no_cols; i++) {
            if (line[i] == 'S') {
                start_particle_pos = i;
            }
            if (line[i] == '^') {
                add_splitter(no_rows, i);
            }
        }
        no_rows++;
    }
}

void add_splitter(size_t row, size_t col) {
    rows[row].splitters[rows[row].no_splitters++] = col;
}

u_int64_t count_visited() {
    u_int64_t count = 0;
    for (size_t r = 0; r < no_rows; r++) {
        for (size_t s = 0; s < rows[r].no_splitters; s++) {
            if (rows[r].visited[s]) {
                count++;
            }
        }
    }
    return count;
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
