#ifndef CARGACRITERIOS_H
#define CARGACRITERIOS_H

#include "criteria.h"

// void menuCargarCriterios(criterioBusqueda *c);
int leerCriterioDesdeArchivo(criterioBusqueda *c, const char *ruta);
int guardarCriteriosEnJSON(criterioBusqueda *c, const char *nombreArchivo);

#endif