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

int max_joltage(int *bank, int n) {
    int max = 0, max2 = 0, max_index = -1;
    for (int i = 0; i < n-1; i++) {
        if (bank[i] > max) {
            max = bank[i];
            max_index = i;
        }
    }
    for (int i = max_index + 1; i < n; i++) {
        if (bank[i] > max2) {
            max2 = bank[i];
        }
    }
    return max * 10 + max2;
}

void parse_file(FILE *f) {
    int count1 = 0;
    int c = fgetc(f);
    while (c != EOF) {
        int bank[101];
        int i = 0;
        while (c != '\n') {
            bank[i++] = c - '0';
            c = fgetc(f);
        }
        int j = max_joltage(bank, i);
        count1 += j;
        c = fgetc(f);
    }
    printf("%d\n", count1);
}
