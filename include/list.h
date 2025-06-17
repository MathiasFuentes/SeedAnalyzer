#ifndef LIST_H
#define LIST_H

typedef struct Node {
  void *data;
  struct Node *next;
} Node;

struct List {
  Node *head;
  Node *tail;
  Node *current;
  int size;
};

typedef List List;

// Esta función crea una lista vacía y devuelve un puntero a la lista.
List *list_create();

// Esta función devuelve un puntero al primer elemento de la lista.
void *list_first(List *L);

// Esta función mueve el puntero de la lista al siguiente elemento y devuelve un
// puntero a dicho elemento.
void *list_next(List *L);

// Esta función inserta un nuevo elemento al inicio de la lista.
void list_pushFront(List *L, void *dato);

// Esta función inserta un nuevo elemento al final de la lista.
void list_pushBack(List *L, void *dato);

// Esta función inserta un nuevo elemento a continuación del actual de la lista.
void list_pushCurrent(List *L, void *dato);

// Esta función elimina el primer elemento de la lista.
void *list_popFront(List *L);

// Esta función elimina el último elemento de la lista.
void *list_popBack(List *L);

// Esta función elimina el elemento actual de la lista.
void *list_popCurrent(List *L);

// Esta función elimina todos los elementos de la lista.
void list_clean(List *L);

// Función para insertar ordenado de acuerdo a la función lower_than
void list_sortedInsert(List *L, void *data, int (*lower_than)(void *data1, void *data2));

// Función para saber si es que existe un elemento en la lista
int list_exist(List *lista, void *dato, int (*comparar)(void *, void *));

// Función que devuelve el size de una lista.
int list_size(List* L);

// Función que libera la memoria de una lista.
void list_destroy_with(List* list, void (*free_data)(void*));

#endif