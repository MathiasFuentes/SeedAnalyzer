#include "map.h"
#include <stdlib.h>
#include <string.h>

// Internal entry
typedef struct Entry {
    char *key;
    void *value;
    struct Entry *next;
} Entry;

struct Map {
    Entry **buckets;
    size_t capacity;
    size_t size;
    hash_func hf;
    key_eq_func eqf;
};

// Default string hash (djb2)
static unsigned long default_hash(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash;
}

static bool default_eq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

Map *map_create(size_t capacity, hash_func hf, key_eq_func eqf) {
    Map *m = malloc(sizeof(Map));
    if (!m) return NULL;
    m->capacity = capacity;
    m->size = 0;
    m->hf = hf ? hf : default_hash;
    m->eqf = eqf ? eqf : default_eq;
    m->buckets = calloc(capacity, sizeof(Entry*));
    if (!m->buckets) { free(m); return NULL; }
    return m;
}

void map_destroy(Map *m, bool free_values) {
    if (!m) return;
    for (size_t i = 0; i < m->capacity; i++) {
        Entry *e = m->buckets[i];
        while (e) {
            Entry *next = e->next;
            free(e->key);
            if (free_values && e->value) free(e->value);
            free(e);
            e = next;
        }
    }
    free(m->buckets);
    free(m);
}

bool map_put(Map *m, const char *key, void *value) {
    unsigned long h = m->hf(key) % m->capacity;
    Entry *e = m->buckets[h];
    while (e) {
        if (m->eqf(e->key, key)) {
            e->value = value;
            return false;
        }
        e = e->next;
    }
    // insert new
    Entry *n = malloc(sizeof(Entry));
    n->key = strdup(key);
    n->value = value;
    n->next = m->buckets[h];
    m->buckets[h] = n;
    m->size++;
    return true;
}

void *map_get(const Map *m, const char *key) {
    unsigned long h = m->hf(key) % m->capacity;
    Entry *e = m->buckets[h];
    while (e) {
        if (m->eqf(e->key, key)) return e->value;
        e = e->next;
    }
    return NULL;
}

bool map_remove(Map *m, const char *key, bool free_value) {
    unsigned long h = m->hf(key) % m->capacity;
    Entry *e = m->buckets[h];
    Entry *prev = NULL;
    while (e) {
        if (m->eqf(e->key, key)) {
            if (prev) prev->next = e->next;
            else m->buckets[h] = e->next;
            free(e->key);
            if (free_value && e->value) free(e->value);
            free(e);
            m->size--;
            return true;
        }
        prev = e;
        e = e->next;
    }
    return false;
}

size_t map_size(const Map *m) {
    return m->size;
}