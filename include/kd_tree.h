#ifndef KD_TREE_H
#define KD_TREE_H

#include <stddef.h>
#include <stdbool.h>

// A 2D point
typedef struct {
    double x;
    double z;
    void *data;  // optional payload
} KDPoint;

// Opaque KD-Tree structure
typedef struct KDTree KDTree;

// Create a KD-Tree with initial capacity hint
KDTree *kd_create(size_t capacity);

// Destroy KD-Tree and free nodes. If free_data is true, free the data pointers too.
void kd_destroy(KDTree *tree, bool free_data);

// Insert a point into the tree
bool kd_insert(KDTree *tree, KDPoint point);

// Find the nearest neighbor to the target point; returns true if found
bool kd_nearest(const KDTree *tree, KDPoint target, KDPoint *out);

// Number of points in the tree
size_t kd_size(const KDTree *tree);

#endif // KD_TREE_H
