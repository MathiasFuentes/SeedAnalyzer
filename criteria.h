#ifndef CRITERIA_H
#define CRITERIA_H

#include "list.h"

typedef struct{
    long long numeroAldeas;
    long long radioBusquedaEnChunks;
    double coordenadasIniciales[2];
    List *biomasRequeridos;
    List *estructurasRequeridas;
} criterioBusqueda;

typedef struct BiomaNombre {
    char nombreEsp[64];
    char nombreCub[64];
} BiomaNombre;

criterioBusqueda* setDefaultCriterio();

int compararBiomas(void *bioma1, void *bioma2);

void mostrarBiomasSeleccionados(criterioBusqueda *c);

void agregarBioma(criterioBusqueda *c, BiomaNombre listaBiomas[]);

void eliminarBioma(criterioBusqueda *c);

void mostrarBiomasDisponibles(BiomaNombre listaBiomas[]);

void almacenarBiomas(criterioBusqueda *c);

void almacenarCriterios(criterioBusqueda *c);

void guardarCriteriosEnJSON(criterioBusqueda *c);


#endif