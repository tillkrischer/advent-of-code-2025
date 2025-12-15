#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    int *parent;      // parent[i] = parent of node i
    int *rank;        // rank[i] = approximate depth of tree rooted at i
    int *size;        // size[i] = size of set rooted at i (only valid for roots)
    int n;            // number of elements
    int num_sets;     // number of distinct sets
} UnionFind;

// Create a new union-find structure with n elements (0 to n-1)
UnionFind *uf_create(int n);

// Find the root of element x with path compression
int uf_find(UnionFind *uf, int x);

// Union the sets containing x and y
// Returns true if they were in different sets, false otherwise
bool uf_union(UnionFind *uf, int x, int y);

// Check if x and y are in the same set
bool uf_connected(UnionFind *uf, int x, int y);

// Get the size of the set containing x
int uf_get_size(UnionFind *uf, int x);

// Get the number of distinct sets
int uf_num_sets(UnionFind *uf);

// Free the union-find structure
void uf_free(UnionFind *uf);

#endif // UNION_FIND_H
