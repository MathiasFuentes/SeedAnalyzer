#ifndef CRITERIA_H
#define CRITERIA_H

#include <list.h>

typedef struct{
    long long numeroAldeas;
    long long radioBusquedaEnChunks;
    double coordenadasIniciales[2];
    List *biomasRequeridos;
    List *estructurasRequeridas;
} criterioBusqueda;

typedef struct BiomaNombre{
    char *nombreEsp;
    char *nombreCub;
} BiomaNombre;

criterioBusqueda* setDefaultCriterio();

void almacenarBioma(criterioBusqueda *c);

void almacenarCriterios(criterioBusqueda *c);

void guardarCriteriosEnJSON(criterioBusqueda *c);


#endif