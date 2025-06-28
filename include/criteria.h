#ifndef CRITERIA_H
#define CRITERIA_H

#define TOTAL_BIOMAS 10
#define TOTAL_ESTRUCTURAS 7


/*
     Estructura ID:
     Representa una entidad del juego (bioma o estructura) con su nombre en español (nombreEsp)
     y su nombre técnico en inglés/código (nombreCub).
 */
typedef struct { char nombreEsp[32]; char nombreCub[32]; } ID;

extern ID Biomas[TOTAL_BIOMAS];
extern ID Estructuras[TOTAL_ESTRUCTURAS];

#include "list.h"


/*
     Estructura criterioBusqueda:
     Representa todos los criterios que el usuario puede definir para buscar una zona óptima.
     Contiene listas para:
     - Rango de búsqueda en chunks
     - Coordenadas iniciales (X, Z)
     - Biomas requeridos
     - Estructuras requeridas
 */
typedef struct{
    List *radioBusquedaEnChunks;
    List *coordenadasIniciales;
    List *biomasRequeridos;
    List *estructurasRequeridas;
} criterioBusqueda;


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

void resumenCriterios(criterioBusqueda *c, int flag);

#endif