#include <stdio.h>
#include <string.h>

#define NUM_PRESENTS 6

typedef struct {
    int width;
    int height;
    int present_counts[NUM_PRESENTS];
} Grid;

bool parse_file(const char *filename);
bool does_fit(Grid *grid);

int present_areas[NUM_PRESENTS];
Grid grids[1000];
int grid_count = 0;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    if (!parse_file(argv[1])) {
        return 1;
    }

    int fitting_grids = 0;
    for (int i = 0; i < grid_count; i++) {
        if (does_fit(&grids[i])) {
            fitting_grids++;
        }
    }
    printf("Number of fitting grids: %d\n", fitting_grids);

    return 0;
}

bool does_fit(Grid *grid) {
    int available_area = grid->width * grid->height;
    int required_area = 0;
    for (int i = 0; i < NUM_PRESENTS; i++) {
        required_area += grid->present_counts[i] * present_areas[i];
    }
    return required_area <= available_area;
}

void parse_grid(char *line) {
    Grid *grid = &grids[grid_count++];
    sscanf(line, "%dx%d: %d %d %d %d %d %d", &grid->width, &grid->height,
           &grid->present_counts[0], &grid->present_counts[1],
           &grid->present_counts[2], &grid->present_counts[3],
           &grid->present_counts[4], &grid->present_counts[5]);
}

void parse_presents(FILE *f) {
    char line[256];
    for (int i = 0; i < NUM_PRESENTS; i++) {
        present_areas[i] = 0;
        fgets(line, sizeof(line), f);
        for (int j = 0; j < 3; j++) {
            fgets(line, sizeof(line), f);
            for (int k = 0; k < 3; k++) {
                if (line[k] == '#') {
                    present_areas[i]++;
                }
            }
        }
        fgets(line, sizeof(line), f);
    }
}

void parse_input(FILE *f) {
    parse_presents(f);
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        parse_grid(line);
    }
}

bool parse_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return false;
    }

    parse_input(f);

    if (ferror(f)) {
        perror("read error");
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}
