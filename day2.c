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

bool is_repeat(unsigned long long n) {
    int length = floor(log10(n)) + 1;
    if (length % 2 == 1) {
        return false;
    }
    unsigned long long divisor = pow(10, (int)(length / 2));
    unsigned long long first = n / divisor;
    unsigned long long second = n % divisor;

    return first == second;
}

void parse_file(FILE *f) {
    unsigned long long start, end, count1 = 0;
    do {
        fscanf(f, "%llu-%llu", &start, &end);
        for (unsigned long long i = start; i <= end; i++) {
            if (is_repeat(i)) {
                count1 += i;
            }
        }
    } while (fgetc(f) == ',');
    printf("%llu\n", count1);
}
