#ifndef CARGACRITERIOS_H
#define CARGACRITERIOS_H

#include "criteria.h"

int leerCriterioDesdeArchivo(criterioBusqueda *c, const char *ruta);
int guardarCriteriosEnJSON(criterioBusqueda *c, const char *nombreArchivo);

#endif
