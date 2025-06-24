#include "kd_tree.h"
#include <stdlib.h>
#include <math.h>

#define K 2

typedef struct KDNode {
    KDPoint point;
    struct KDNode *left;
    struct KDNode *right;
} KDNode;

struct KDTree {
    KDNode *root;
    size_t count;
};

static KDNode *node_create(KDPoint pt) {
    KDNode *n = malloc(sizeof(KDNode));
    if (!n) return NULL;
    n->point = pt;
    n->left = n->right = NULL;
    return n;
}

KDTree *kd_create(size_t capacity) {
    (void)capacity; // capacity hint unused in this simple implementation
    KDTree *t = malloc(sizeof(KDTree));
    if (!t) return NULL;
    t->root = NULL;
    t->count = 0;
    return t;
}

static void node_destroy(KDNode *n, bool free_data) {
    if (!n) return;
    node_destroy(n->left, free_data);
    node_destroy(n->right, free_data);
    if (free_data && n->point.data) free(n->point.data);
    free(n);
}

void kd_destroy(KDTree *tree, bool free_data) {
    if (!tree) return;
    node_destroy(tree->root, free_data);
    free(tree);
}

static KDNode *insert_rec(KDNode *node, KDPoint pt, unsigned depth, bool *inserted) {
    if (!node) {
        *inserted = true;
        return node_create(pt);
    }
    unsigned axis = depth % K;
    double diff = (axis == 0 ? pt.x - node->point.x : pt.z - node->point.z);
    if (diff < 0) node->left = insert_rec(node->left, pt, depth + 1, inserted);
    else node->right = insert_rec(node->right, pt, depth + 1, inserted);
    return node;
}

bool kd_insert(KDTree *tree, KDPoint point) {
    if (!tree) return false;
    bool inserted = false;
    tree->root = insert_rec(tree->root, point, 0, &inserted);
    if (inserted) tree->count++;
    return inserted;
}

static inline double sqr(double v) { return v * v; }

static void nearest_rec(const KDNode *node, KDPoint target, unsigned depth,
                        const KDNode **best, double *best_dist) {
    if (!node) return;
    double dx = target.x - node->point.x;
    double dz = target.z - node->point.z;
    double dist = sqr(dx) + sqr(dz);
    if (dist < *best_dist) {
        *best_dist = dist;
        *best = node;
    }
    unsigned axis = depth % K;
    double diff = (axis == 0 ? dx : dz);
    const KDNode *first = diff < 0 ? node->left : node->right;
    const KDNode *second = diff < 0 ? node->right : node->left;
    nearest_rec(first, target, depth + 1, best, best_dist);
    if (sqr(diff) < *best_dist) {
        nearest_rec(second, target, depth + 1, best, best_dist);
    }
}

bool kd_nearest(const KDTree *tree, KDPoint target, KDPoint *out) {
    if (!tree || !tree->root || !out) return false;
    const KDNode *best = tree->root;
    double best_dist = sqr(target.x - best->point.x) + sqr(target.z - best->point.z);
    nearest_rec(tree->root, target, 0, &best, &best_dist);
    *out = best->point;
    return true;
}

size_t kd_size(const KDTree *tree) {
    return tree ? tree->count : 0;
}