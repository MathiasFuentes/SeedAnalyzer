/* map.h */
#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Map Map;

typedef unsigned long (*hash_func)(const char *key);

typedef bool (*key_eq_func)(const char *a, const char *b);

// Create a new map with given capacity, hash function and key equality function.
Map *map_create(size_t capacity, hash_func hf, key_eq_func eqf);

// Destroy and free all resources. If free_values is true, free each value.
void map_destroy(Map *m, bool free_values);

// Insert or update: returns true if new key inserted.
bool map_put(Map *m, const char *key, void *value);

// Get value by key, or NULL if not found.
void *map_get(const Map *m, const char *key);

// Remove key: returns true if key existed and was removed.
bool map_remove(Map *m, const char *key, bool free_value);

// Number of entries
size_t map_size(const Map *m);

#endif // MAP_H