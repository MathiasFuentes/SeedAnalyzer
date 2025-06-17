#include "criteria.h"
#include <stdio.h>
#include <stdlib.h>

/*
typedef struct{
    long long numeroAldeas;
    long long radioBusquedaEnChunks;
    double coordenadasIniciales[2];
    char **biomasRequeridos;
    int numeroBiomasRequeridos;
    char **estructurasRequeridas;
    int numeroEstructurasRequeridas;
} criterioBusqueda;
*/

criterioBusqueda *setDefaultCriterio() {
    criterioBusqueda *c = malloc(sizeof(criterioBusqueda));
    c->numeroAldeas = 0;
    c->radioBusquedaEnChunks = 128; // Valor por defecto, puede ser ajustado
    c->coordenadasIniciales[0] = 0.0; // Coordenadas (0,0) por defecto
    c->coordenadasIniciales[1] = 0.0;
    c->biomasRequeridos = NULL;
    c->numeroBiomasRequeridos = 0;
    c->estructurasRequeridas = NULL;
    c->numeroEstructurasRequeridas = 0;
    return c;
}

void almacenarCriterios(criterioBusqueda *c) {
    
}