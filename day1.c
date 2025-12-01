#include <stdio.h>
#include <stdlib.h>

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
    int pos = 50;
    char line[4096];
    int count1 = 0;
    int count2 = 0;
    char *end;
    long v;
    while (fgets(line, sizeof line, f)) {
        v = strtol(line + 1, &end, 10);
        if (line[0] == 'L') {
            v = -v;
        }
        count2 += labs(v) / 100;
        v %= 100;
        if (pos != 0 && (v + pos <= 0 || v + pos >= 100)) {
            count2 += 1;
        }
        pos = (pos + v + 100) % 100;

        if (pos == 0) {
            count1 += 1;
        }
    }
    printf("%d\n", count1);
    printf("%d\n", count2);
}

