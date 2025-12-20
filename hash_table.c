#include "hash_table.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR_THRESHOLD 0.75

// FNV-1a hash function
static size_t hash_string(const char *key) {
    size_t hash = 14695981039346656037ULL;  // FNV offset basis
    while (*key) {
        hash ^= (unsigned char)*key++;
        hash *= 1099511628211ULL;  // FNV prime
    }
    return hash;
}

static HashEntry *entry_create(const char *key, int value) {
    HashEntry *entry = malloc(sizeof(HashEntry));
    if (!entry) return NULL;
    
    entry->key = strdup(key);
    if (!entry->key) {
        free(entry);
        return NULL;
    }
    
    entry->value = value;
    entry->next = NULL;
    return entry;
}

static void entry_free(HashEntry *entry) {
    if (entry) {
        free(entry->key);
        free(entry);
    }
}

static void ht_resize(HashTable *ht) {
    size_t new_capacity = ht->capacity * 2;
    HashEntry **new_buckets = calloc(new_capacity, sizeof(HashEntry *));
    if (!new_buckets) return;  // Resize failed, continue with current capacity
    
    // Rehash all entries
    for (size_t i = 0; i < ht->capacity; i++) {
        HashEntry *entry = ht->buckets[i];
        while (entry) {
            HashEntry *next = entry->next;
            size_t index = hash_string(entry->key) % new_capacity;
            entry->next = new_buckets[index];
            new_buckets[index] = entry;
            entry = next;
        }
    }
    
    free(ht->buckets);
    ht->buckets = new_buckets;
    ht->capacity = new_capacity;
}

HashTable *ht_create(void) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (!ht) return NULL;
    
    ht->buckets = calloc(INITIAL_CAPACITY, sizeof(HashEntry *));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }
    
    ht->capacity = INITIAL_CAPACITY;
    ht->size = 0;
    return ht;
}

void ht_set(HashTable *ht, const char *key, int value) {
    // Check if resize needed
    if ((double)ht->size / ht->capacity >= LOAD_FACTOR_THRESHOLD) {
        ht_resize(ht);
    }
    
    size_t index = hash_string(key) % ht->capacity;
    
    // Check if key already exists
    HashEntry *entry = ht->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;  // Update existing
            return;
        }
        entry = entry->next;
    }
    
    // Insert new entry at head of chain
    HashEntry *new_entry = entry_create(key, value);
    if (!new_entry) return;
    
    new_entry->next = ht->buckets[index];
    ht->buckets[index] = new_entry;
    ht->size++;
}

int *ht_get(HashTable *ht, const char *key) {
    size_t index = hash_string(key) % ht->capacity;
    
    HashEntry *entry = ht->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return &entry->value;
        }
        entry = entry->next;
    }
    
    return NULL;
}

bool ht_contains(HashTable *ht, const char *key) {
    return ht_get(ht, key) != NULL;
}

bool ht_remove(HashTable *ht, const char *key) {
    size_t index = hash_string(key) % ht->capacity;
    
    HashEntry *entry = ht->buckets[index];
    HashEntry *prev = NULL;
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                ht->buckets[index] = entry->next;
            }
            entry_free(entry);
            ht->size--;
            return true;
        }
        prev = entry;
        entry = entry->next;
    }
    
    return false;
}

size_t ht_size(HashTable *ht) {
    return ht->size;
}

void ht_free(HashTable *ht) {
    if (ht) {
        for (size_t i = 0; i < ht->capacity; i++) {
            HashEntry *entry = ht->buckets[i];
            while (entry) {
                HashEntry *next = entry->next;
                entry_free(entry);
                entry = next;
            }
        }
        free(ht->buckets);
        free(ht);
    }
}
