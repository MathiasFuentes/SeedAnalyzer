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

typedef struct ID{
    char nombreEsp[64];
    char nombreCub[64];
} ID;

criterioBusqueda* setDefaultCriterio();

int compararID(void *ID1, void *ID2);

void mostrarBiomasSeleccionados(criterioBusqueda *c);

void agregarBioma(criterioBusqueda *c, ID listaBiomas[]);

void eliminarBioma(criterioBusqueda *c);

void mostrarBiomasDisponibles(ID listaBiomas[]);

void almacenarBiomas(criterioBusqueda *c);

void almacenarCriterios(criterioBusqueda *c);

void guardarCriteriosEnJSON(criterioBusqueda *c);


#endif