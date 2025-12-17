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

int x_cords[500];
size_t no_x_cords = 0;
int y_cords[500];
size_t no_y_cords = 0;

bool parse_file(const char *filename);
void parse_input(FILE *f);
u_int64_t area(Point a, Point b);
void get_largest_area(u_int64_t *largest_area,
                      u_int64_t *largest_contained_area);
bool is_rect_contained(Point a, Point b);
bool is_point_contained(Point p);
bool is_on_any_edge(Point p);
bool is_inside_poly(Point p);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    if (!parse_file(argv[1])) {
        return 1;
    }

    u_int64_t largest_area = 0, largest_contained_area = 0;
    get_largest_area(&largest_area, &largest_contained_area);
    printf("Largest area: %llu\n", largest_area);
    printf("Largest contained area: %llu\n", largest_contained_area);

    return 0;
}

void get_largest_area(u_int64_t *largest_area,
                      u_int64_t *largest_contained_area) {
    for (size_t i = 0; i < no_points; i++) {
        for (size_t j = i + 1; j < no_points; j++) {
            u_int64_t a = area(points[i], points[j]);
            if (a > *largest_area) {
                *largest_area = a;
            }
            if (a > *largest_contained_area &&
                is_rect_contained(points[i], points[j])

            ) {
                // printf("New largest contained area %llu with points (%d, %d) "
                //        "and (%d, %d)\n",
                //        a, points[i].x, points[i].y, points[j].x, points[j].y);
                *largest_contained_area = a;
            }
        }
    }
}

bool is_rect_contained(Point a, Point b) {
    long min_x = a.x < b.x ? a.x : b.x;
    long max_x = a.x > b.x ? a.x : b.x;
    long min_y = a.y < b.y ? a.y : b.y;
    long max_y = a.y > b.y ? a.y : b.y;

    size_t i = 0;
    while (i < no_x_cords && x_cords[i] < min_x) {
        i++;
    }
    while (i < no_x_cords && x_cords[i] <= max_x) {
        Point p1 = {.x = x_cords[i], .y = min_y};
        Point p2 = {.x = x_cords[i], .y = max_y};
        if (!is_point_contained(p1) || !is_point_contained(p2)) {
            return false;
        }
        i++;
    }

    i = 0;
    while (i < no_y_cords && y_cords[i] < min_y) {
        i++;
    }
    while (i < no_y_cords && y_cords[i] <= max_y) {
        Point p1 = {.x = min_x, .y = y_cords[i]};
        Point p2 = {.x = max_x, .y = y_cords[i]};
        if (!is_point_contained(p1) || !is_point_contained(p2)) {
            return false;
        }
        i++;
    }

    return true;
}

bool is_point_contained(Point p) {
    if (is_on_any_edge(p)) {
        return true;
    }
    if (is_inside_poly(p)) {
        return true;
    }
    return false;
}

bool is_inside_poly(Point p) {
    double cx = p.x + 0.5;
    double cy = p.y + 0.5;
    int intersections = 0;
    for (size_t i = 0; i < no_points; i++) {
        Point start = points[i];
        Point end = points[(i + 1) % no_points];
        if (start.y == end.y) {
            int min_x = start.x < end.x ? start.x : end.x;
            int max_x = start.x > end.x ? start.x : end.x;
            if (cy < start.y && cx > min_x && cx < max_x) {
                intersections++;
            }
        }
    }
    return (intersections % 2) == 1;
}

bool is_on_any_edge(Point p) {
    for (size_t i = 0; i < no_points; i++) {
        Point start = points[i];
        Point end = points[(i + 1) % no_points];
        long min_x = start.x < end.x ? start.x : end.x;
        long max_x = start.x > end.x ? start.x : end.x;
        long min_y = start.y < end.y ? start.y : end.y;
        long max_y = start.y > end.y ? start.y : end.y;
        if (p.x >= min_x && p.x <= max_x && p.y >= min_y && p.y <= max_y) {
            return true;
        }
    }
    return false;
}

u_int64_t area(Point a, Point b) {
    u_int64_t x = abs(b.x - a.x) + 1;
    u_int64_t y = abs(b.y - a.y) + 1;
    return x * y;
}

int compare_ints_asc(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int make_unique(int *arr, int size) {
    if (size <= 1) {
        return size;
    }
    qsort(arr, size, sizeof(int), compare_ints_asc);

    int unique_count = 1;
    for (int i = 1; i < size; i++) {
        if (arr[i] != arr[i - 1]) {
            arr[unique_count++] = arr[i];
        }
    }

    return unique_count;
}

void parse_input(FILE *f) {
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        int x, y;
        sscanf(line, "%d,%d\n", &x, &y);
        points[no_points].x = x;
        points[no_points].y = y;
        x_cords[no_x_cords++] = x;
        y_cords[no_y_cords++] = y;
        no_points++;
    }
    no_x_cords = make_unique(x_cords, no_x_cords);
    no_y_cords = make_unique(y_cords, no_y_cords);
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
