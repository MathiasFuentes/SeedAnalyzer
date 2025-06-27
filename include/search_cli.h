#ifndef SEARCH_CLI_H
#define SEARCH_CLI_H

#include "criteria.h"
#include "search.h"
#include "kd_tree.h"
#include <stdint.h>
#include <stdbool.h>

bool cumpleRequisitosBusqueda(criterioBusqueda *c, uint64_t *pseed);

int realizarBusqueda(criterioBusqueda *c,
                     uint64_t *pseed,
                     RegionResult **outArr,
                     int *outCount);
                     
uint64_t generate_random_seed(void);

void modificarSemilla(uint64_t *pseed);

void mostrarResultados(const criterioBusqueda *c,
                       uint64_t seed,
                       const RegionResult *arr,
                       int count,
                       KDTree *tree);


#endif