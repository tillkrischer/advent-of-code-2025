#include <stdio.h>
#include <stdlib.h>

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

    int pos = 50;
    char line[4096];
    int count = 0;
    char *end;
    long value;
    while (fgets(line, sizeof line, f)) {
        value = strtol(line+1, &end, 10);
        if (line[0] == 'L') {
            pos -= value % 100;
        } else if (line[0] == 'R') {
            pos += value % 100;
        }
        pos %= 100;
        if (pos == 0) {
            count += 1;
        }
	}
    printf("%d\n", count);
    if (ferror(f)) {
        perror("read error");
    }

    fclose(f);
    return 0;
}
