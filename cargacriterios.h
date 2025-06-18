#ifndef CARGA_CRITERIOS_H
#define CARGA_CRITERIOS_H

#include "criteria.h"

void menuCargarCriterios(criterioBusqueda *c);
int leerCriterioDesdeArchivo(criterioBusqueda *c, const char *ruta);

#endif