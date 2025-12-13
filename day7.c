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
    char line[256];
    char prev_line[256];
    size_t cols;
    fgets(prev_line, sizeof(prev_line), f);
    cols = strlen(prev_line);
    u_int64_t splits = 0;
    while (fgets(line, sizeof(line), f)) {
        for (size_t i = 0; i < cols; i++) {
            if (line[i] == '^' && prev_line[i] == '|') {
                line[i - 1] = '|';
                line[i + 1] = '|';
                splits++;
            } else if (prev_line[i] == 'S' || prev_line[i] == '|') {
                line[i] = '|';
            }
        }
        strcpy(prev_line, line);
    }
    printf("%llu\n", splits);
}
