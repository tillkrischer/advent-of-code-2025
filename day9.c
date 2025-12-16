#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int x;
    int y;
} Point;
Point points[500];
size_t no_points = 0;

bool parse_file(const char *filename);
void parse_input(FILE *f);
u_int64_t area(Point a, Point b);
u_int64_t get_largest_area();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    if (!parse_file(argv[1])) {
        return 1;
    }

    u_int64_t largest_area = get_largest_area();
    printf("Largest area: %llu\n", largest_area);

    return 0;
}

u_int64_t get_largest_area() {
    u_int64_t largest_area = 0;
    for (size_t i = 0; i < no_points; i++) {
        for (size_t j = i + 1; j < no_points; j++) {
            u_int64_t a = area(points[i], points[j]);
            if (a > largest_area) {
                // printf("New largest area %llu between points (%d,%d) and (%d,%d)\n",
                //        a, points[i].x, points[i].y, points[j].x, points[j].y);
                largest_area = a;
            }
        }
    }
    return largest_area;
}

u_int64_t area(Point a, Point b) {
    u_int64_t x = abs(b.x - a.x ) + 1;
    u_int64_t y = abs(b.y - a.y ) + 1;
    return x * y;
}

void parse_input(FILE *f) {
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%d,%d\n", &points[no_points].x, &points[no_points].y);
        no_points++;
    }
    printf("Parsed %zu points\n", no_points);
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
