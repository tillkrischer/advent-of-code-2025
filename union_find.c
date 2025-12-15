#include "union_find.h"
#include <stdlib.h>

UnionFind *uf_create(int n) {
    UnionFind *uf = malloc(sizeof(UnionFind));
    if (!uf) return NULL;
    
    uf->parent = malloc(n * sizeof(int));
    uf->rank = malloc(n * sizeof(int));
    uf->size = malloc(n * sizeof(int));
    
    if (!uf->parent || !uf->rank || !uf->size) {
        free(uf->parent);
        free(uf->rank);
        free(uf->size);
        free(uf);
        return NULL;
    }
    
    uf->n = n;
    uf->num_sets = n;
    
    // Initialize: each element is its own parent (root)
    for (int i = 0; i < n; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
        uf->size[i] = 1;
    }
    
    return uf;
}

int uf_find(UnionFind *uf, int x) {
    if (uf->parent[x] != x) {
        // Path compression: make x point directly to root
        uf->parent[x] = uf_find(uf, uf->parent[x]);
    }
    return uf->parent[x];
}

bool uf_union(UnionFind *uf, int x, int y) {
    int root_x = uf_find(uf, x);
    int root_y = uf_find(uf, y);
    
    // Already in same set
    if (root_x == root_y) {
        return false;
    }
    
    // Union by rank: attach smaller tree under larger tree
    if (uf->rank[root_x] < uf->rank[root_y]) {
        uf->parent[root_x] = root_y;
        uf->size[root_y] += uf->size[root_x];
    } else if (uf->rank[root_x] > uf->rank[root_y]) {
        uf->parent[root_y] = root_x;
        uf->size[root_x] += uf->size[root_y];
    } else {
        uf->parent[root_y] = root_x;
        uf->size[root_x] += uf->size[root_y];
        uf->rank[root_x]++;
    }
    
    uf->num_sets--;
    return true;
}

bool uf_connected(UnionFind *uf, int x, int y) {
    return uf_find(uf, x) == uf_find(uf, y);
}

int uf_get_size(UnionFind *uf, int x) {
    int root = uf_find(uf, x);
    return uf->size[root];
}

int uf_num_sets(UnionFind *uf) {
    return uf->num_sets;
}

void uf_free(UnionFind *uf) {
    if (uf) {
        free(uf->parent);
        free(uf->rank);
        free(uf->size);
        free(uf);
    }
}
