#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>

typedef struct Heap Heap;

void* heap_top(Heap* pq);

void heap_push(Heap* pq, void* data, int priority);

void heap_pop(Heap* pq);

Heap* heap_create();

bool heap_empty(const Heap *pq);

int heap_size(const Heap *pq);

void heap_destroy(Heap *h);

#endif