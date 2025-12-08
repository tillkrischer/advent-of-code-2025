#include <stdint.h>
#include <stdio.h>
#include <string.h>

void parse_file(FILE *f);

typedef struct {
    char data[256][256];
    int height;
    int width;
} Grid;

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

bool is_in_bounds(Grid *grid, int y, int x) {
    return (y >= 0 && y < grid->height && x >= 0 && x < grid->width);
}

int count_adj(Grid *grid, int y, int x) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) {
                continue;
            }
            if (is_in_bounds(grid, y + dy, x + dx) &&
                grid->data[y + dy][x + dx] == '@') {
                count += 1;
            }
        }
    }
    return count;
}

int remove_rolls(Grid *grid) {
    Grid copy = *grid;
    int removed = 0;
    for (int y = 0; y < grid->height; y++) {
        for (int x = 0; x < grid->width; x++) {
            if (grid->data[y][x] == '@') {
                int adj = count_adj(grid, y, x);
                if (adj < 4) {
                    copy.data[y][x] = '.';
                    removed += 1;
                }
            }
        }
    }
    *grid = copy;
    return removed;
}

void parse_file(FILE *f) {
    Grid grid = {0};
    int row = 0;
    while (fgets(grid.data[row], sizeof(grid.data), f)) {
        row += 1;
    }
    grid.height = row;
    grid.width = strlen(grid.data[0]) - 1;

    int count1 = remove_rolls(&grid);
    int count2 = count1;
    int removed = 0;
    while ((removed = remove_rolls(&grid)) > 0) {
        count2 += removed;
    }

    printf("%d\n", count1);
    printf("%d\n", count2);
}
