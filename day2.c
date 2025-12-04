#include <math.h>
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

bool is_n_repeat(unsigned long long v, int n) {
    int length = floor(log10(v)) + 1;
    if (length % n != 0) {
        return false;
    }
    int part_length = length / n;
    unsigned long long divisor = pow(10, part_length);
    unsigned long long prev_part = v % divisor;
    v /= divisor;
    while (v > 0) {
        unsigned long long part = v % divisor;
        if (part != prev_part) {
            return false;
        }
        prev_part = part;
        v /= divisor;
    }
    return true;
}

bool is_any_repeat(unsigned long long v) {
    int length = floor(log10(v)) + 1;
    for (int parts = 2; parts <= length; parts++) {
        if (is_n_repeat(v, parts)) {
            return true;
        }
    }
    return false;
}

void parse_file(FILE *f) {
    unsigned long long start, end, count1 = 0, count2 = 0;
    do {
        fscanf(f, "%llu-%llu", &start, &end);
        for (unsigned long long i = start; i <= end; i++) {
            if (is_n_repeat(i, 2)) {
                count1 += i;
            }
            if (is_any_repeat(i)) {
                count2 += i;
            }
        }
    } while (fgetc(f) == ',');
    printf("%llu\n", count1);
    printf("%llu\n", count2);
}
