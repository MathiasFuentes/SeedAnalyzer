#ifndef CRITERIA_H
#define CRITERIA_H

#include "list.h"

typedef struct{
    List* radioBusquedaEnChunks;
    List *coordenadasIniciales;
    List *biomasRequeridos;
    List *estructurasRequeridas;
} criterioBusqueda;

typedef struct ID{
    char nombreEsp[64];
    char nombreCub[64];
} ID;

criterioBusqueda* setDefaultCriterio();

void mostrarBiomasDisponibles(ID listaBiomas[]);

void mostrarEstructurasDisponibles(ID listaEstructuras[]);

int compararID(void *ID1, void *ID2);

void almacenarCriterio(List* listaCriterio, char* nombreCriterio, int MAX);

void eliminarSeleccion(List *listaCriterio, char* nombreCriterio);

void eliminarTodasLasSelecciones(List *listaCriterios);

void mostrarSeleccion(List *listaCriterio, char* nombreCriterio, int flag);

void menuOpcionesCriterio(char* nombreCriterio);

int mostrarOpcionesSeleccion(char* nombreCriterio);

void nuevaSeleccion(List* listaCriterio, char* nombreCriterio, int MAX);

void seleccionPorConsola(List* listaCriterio, char* nombreCriterio, int MAX);

void resumenCriterios(criterioBusqueda *c);

#endif