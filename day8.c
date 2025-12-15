#include "union_find.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    u_int64_t x;
    u_int64_t y;
    u_int64_t z;
} Point;
size_t no_points = 0;
Point points[1000];

typedef struct {
    uint64_t a_index;
    uint64_t b_index;
    u_int64_t dist;
} Pair;

u_int64_t dist(Point a, Point b);
bool parse_file(const char *filename);
void parse_input(FILE *f);
int compare_pairs(const void *a, const void *b);
u_int64_t product_of_largest_set_sizes(UnionFind *uf);
u_int64_t product_of_x_cords(Pair p);

Pair pairs[1000000];
size_t no_pairs = 0;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    if (!parse_file(argv[1])) {
        return 1;
    }

    printf("Read %zu points\n", no_points);

    for (size_t i = 0; i < no_points; i++) {
        for (size_t j = i + 1; j < no_points; j++) {
            pairs[no_pairs].a_index = i;
            pairs[no_pairs].b_index = j;
            pairs[no_pairs].dist = dist(points[i], points[j]);
            no_pairs++;
        }
    }

    printf("Computed %zu pairs\n", no_pairs);

    qsort(pairs, no_pairs, sizeof(Pair), compare_pairs);

    // for (size_t i = 0; i < no_pairs - 1; i++) {
    //     printf("%llu,%llu: %llu\n", pairs[i].a_index, pairs[i].b_index,
    //            pairs[i].dist);
    // }

    size_t to_connect = 1000;
    if (no_points == 20) {
        to_connect = 10;
    }

    UnionFind *uf = uf_create(no_points);
    for (size_t i = 0; i < to_connect; i++) {
        uf_union(uf, pairs[i].a_index, pairs[i].b_index);
    }

    printf("connected %zu pairs\n", to_connect);

    u_int64_t product = product_of_largest_set_sizes(uf);
    printf("Product of sizes of three largest sets: %llu\n", product);

    while (uf_num_sets(uf) > 1) {
        uf_union(uf, pairs[to_connect].a_index, pairs[to_connect].b_index);
        to_connect++;
    }

    printf("All points connected after %zu pairs\n", to_connect);

    u_int64_t product2 = product_of_x_cords(pairs[to_connect - 1]);
    printf("Product of x coordinates of last connected pair: %llu\n", product2);

    uf_free(uf);

    return 0;
}

u_int64_t product_of_x_cords(Pair p) {
    return (points[p.a_index].x) * (points[p.b_index].x);
}

u_int64_t product_of_largest_set_sizes(UnionFind *uf) {
    int largest[3] = {0, 0, 0};

    for (int i = 0; i < uf->n; i++) {
        if (uf_find(uf, i) == i) { // i is a root
            int size = uf_get_size(uf, i);

            if (size > largest[0]) {
                largest[2] = largest[1];
                largest[1] = largest[0];
                largest[0] = size;
            } else if (size > largest[1]) {
                largest[2] = largest[1];
                largest[1] = size;
            } else if (size > largest[2]) {
                largest[2] = size;
            }
        }
    }

    return largest[0] * largest[1] * largest[2];
}

int compare_pairs(const void *a, const void *b) {
    const Pair *pa = (const Pair *)a;
    const Pair *pb = (const Pair *)b;
    if (pa->dist < pb->dist)
        return -1;
    if (pa->dist > pb->dist)
        return 1;
    return 0;
}

u_int64_t dist(Point a, Point b) {
    u_int64_t x2 = (a.x - b.x) * (a.x - b.x);
    u_int64_t y2 = (a.y - b.y) * (a.y - b.y);
    u_int64_t z2 = (a.z - b.z) * (a.z - b.z);
    return x2 + y2 + z2;
}

void parse_input(FILE *f) {
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (no_points >= sizeof(points) / sizeof(points[0])) {
            fprintf(stderr, "Too many points\n");
            exit(1);
        }

        Point p;
        if (sscanf(line, "%llu,%llu,%llu", &p.x, &p.y, &p.z) != 3) {
            fprintf(stderr, "Invalid line: %s", line);
            exit(1);
        }

        points[no_points++] = p;
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
