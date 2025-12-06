#include <math.h>
#include <stdint.h>
#include <stdio.h>

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

uint64_t max_joltage(int *bank, int bank_size, int start, int remaining) {
    int max = 0, max_index = -1;
    uint64_t res = 0;
    for (int i = start; i < bank_size - remaining+1; i++) {
        if (bank[i] > max) {
            max = bank[i];
            max_index = i;
        }
    }
    res = max * pow(10, remaining - 1);
    if (remaining > 1) {
        res += max_joltage(bank, bank_size, max_index + 1, remaining - 1);
    }
    return res;
}

void parse_file(FILE *f) {
    uint64_t count1 = 0, count2 = 0;
    int c = fgetc(f);
    while (c != EOF) {
        int bank[101];
        int i = 0;
        while (c != '\n') {
            bank[i++] = c - '0';
            c = fgetc(f);
        }
        count1 += max_joltage(bank, i, 0, 2);
        count2 += max_joltage(bank, i, 0, 12);
        c = fgetc(f);
    }
    printf("%llu\n", count1);
    printf("%llu\n", count2);
}
