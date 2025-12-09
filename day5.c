#include <stdint.h>
#include <stdio.h>
#include <string.h>

void parse_file(FILE *f);

typedef struct {
    uint64_t start;
    uint64_t end;
} Range;

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

bool is_fresh(Range *ranges, size_t range_count, uint64_t num) {
    for (size_t i = 0; i < range_count; i++) {
        if (num >= ranges[i].start && num <= ranges[i].end) {
            return true;
        }
    }
    return false;
}

void parse_file(FILE *f) {
    char line[256];
    Range ranges[200];
    size_t range_count = 0;
    uint64_t start, end, num;
    uint64_t count1 = 0;
    while (fgets(line, sizeof(line), f) && line[0] != '\n') {
        sscanf(line, "%llu-%llu", &start, &end);
        ranges[range_count].start = start;
        ranges[range_count].end = end;
        range_count++;
    }

    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%llu", &num);
        if (is_fresh(ranges, range_count, num)) {
            count1++;
        }
    }
    printf("%llu\n", count1);
}
