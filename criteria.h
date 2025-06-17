#ifndef CRITERIA_H
#define CRITERIA_H

typedef struct{
    long long numeroAldeas;
    long long radioBusquedaEnChunks;
    double coordenadasIniciales[2];
    char **biomasRequeridos;
    int numeroBiomasRequeridos;
    char **estructurasRequeridas;
    int numeroEstructurasRequeridas;
} criterioBusqueda;

criterioBusqueda *setDefaultCriterio() {};

void almacenarCriterios(criterioBusqueda *c) {};

void guardarCriteriosEnJSON(criterioBusqueda *c) {}


#endif