#include <stdint.h>
#include <stdio.h>
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

bool is_in_bounds(int H, int W, int y, int x) {
    return (y >= 0 && y < H && x >= 0 && x < W);
}

int count_adj(char line[256][256], int H, int W, int y, int x) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) {
                continue;
            }
            if (is_in_bounds(H, W, y + dy, x + dx) &&
                line[y + dy][x + dx] == '@') {
                count += 1;
            }
        }
    }
    return count;
}

void parse_file(FILE *f) {
    char line[256][256];
    int H = 0, W = 0;
    while (fgets(line[H], sizeof(line), f)) {
        H += 1;
    }
    W = strlen(line[0]) - 1;

    int count1 = 0;
    for(int y = 0; y < H; y++) {
        for(int x = 0; x < W; x++) {
            if (line[y][x] == '@') {
                int adj = count_adj(line, H, W, y, x);
                if (adj < 4) {
                    count1 += 1;
                }
            }
        }
    }
    printf("%d\n", count1);
}
