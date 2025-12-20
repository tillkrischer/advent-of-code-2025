#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct HashEntry {
    char *key;
    int value;
    struct HashEntry *next;
} HashEntry;

typedef struct {
    HashEntry **buckets;
    size_t capacity;
    size_t size;
} HashTable;

// Create a new hash table
HashTable *ht_create(void);

// Insert or update a key-value pair
void ht_set(HashTable *ht, const char *key, int value);

// Get value for key (returns NULL if key doesn't exist)
int *ht_get(HashTable *ht, const char *key);

// Check if key exists
bool ht_contains(HashTable *ht, const char *key);

// Remove a key (returns true if key was found and removed)
bool ht_remove(HashTable *ht, const char *key);

// Get number of entries
size_t ht_size(HashTable *ht);

// Free the hash table
void ht_free(HashTable *ht);

#endif // HASH_TABLE_H
