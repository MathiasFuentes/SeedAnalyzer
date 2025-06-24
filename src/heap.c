#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include "heap.h"

typedef struct nodo{
   void* data;
   int priority;
}heapElem;

typedef struct Heap{
  heapElem* heapArray;
  int size;
  int capac;
} Heap;


void* heap_top(Heap* pq){
    if(pq->size==0) return NULL;
    return pq->heapArray[0].data;
}

void heap_push(Heap* pq, void* data, int priority){

    if(pq->size+1>pq->capac){
        pq->capac=(pq->capac)*2+1;
        pq->heapArray=realloc(pq->heapArray, (pq->capac)*sizeof(heapElem));
    }

    int now = pq->size;
    while(now>0 && pq->heapArray[(now-1)/2].priority < priority)
        {
                pq->heapArray[now] = pq->heapArray[(now-1)/2];
                now = (now -1)/2;
        }
    pq->heapArray[now].priority = priority;
    pq->heapArray[now].data = data;
    pq->size++;
}


void heap_pop(Heap* pq) {
    if (pq->size == 0) return;

    // 1) Mover última hoja a raíz
    pq->size--;
    pq->heapArray[0] = pq->heapArray[pq->size];

    // 2) Hundir el nuevo root
    int idx = 0;
    while (1) {
        int left  = 2*idx + 1;
        int right = 2*idx + 2;
        int largest = idx;

        if (left  < pq->size && pq->heapArray[left ].priority > pq->heapArray[largest].priority)
            largest = left;
        if (right < pq->size && pq->heapArray[right].priority > pq->heapArray[largest].priority)
            largest = right;

        if (largest == idx) 
            break;

        // swap
        heapElem tmp = pq->heapArray[idx];
        pq->heapArray[idx] = pq->heapArray[largest];
        pq->heapArray[largest] = tmp;

        idx = largest;
    }
}

bool heap_empty(const Heap *pq) {
    return pq->size == 0;
}

int heap_size(const Heap *pq) {
    return pq->size;
}

Heap* heap_create(){
   Heap *pq=(Heap*) malloc(sizeof(Heap));
   pq->heapArray=(heapElem*) malloc(3*sizeof(heapElem));
   pq->size=0;
   pq->capac=3; //capacidad inicial
   return pq;
}

void heap_destroy(Heap *pq) {
    if (!pq) return;
    free(pq->heapArray);
    free(pq);
}