#ifndef SEARCH_H
#define SEARCH_H

#include "criteria.h"
#include "generator.h"
#include <stdint.h>
#include <stdbool.h>

int biome_name_to_id(const char *name);
int struct_name_to_type(const char *name);

// Parámetros de usuario para la búsqueda
typedef struct {
    Generator  *gen;           // Generador ya inicializado
    uint64_t    seed;          // Semilla mundial
    int         mcVersion;     // Versión de Minecraft
    int         dim;           // Dimensión (OVERWORLD/NETHER/END)
    int         totalSize;     // Lado N de la zona total (en chunks)
    int         subSize;       // Lado M de cada subregión (en chunks)
    int         startX, startZ;// Centro de búsqueda (chunks)
    int         maxThreads;    // nº de hilos (por si paralelizas)
    uint64_t    biomeMask;     // máscara de biomas requeridos
    uint64_t    structureMask; // máscara de estructuras requeridas
} SearchParams;

// Contexto con todos los buffers intermedios
typedef struct {
    SearchParams params;

    int      numBiomes;        // número de biomas a buscar
    int     *biomeIds;         // array de IDs (enteros) de esos biomas

    int      numStructures;    // número de estructuras a buscar
    int     *structIds;        // array de tipos (enum) de esas estructuras

    int     *biomeGrid;        // [N*N] bioma por chunk
    bool    *structGrid;       // [#structs * N * N] flags por chunk

    int    **prefixBiome;      // [numBiomes][(N+1)*(N+1)]
    int    **prefixStruct;     // [numStructures][(N+1)*(N+1)]
} SearchContext;

// Resultado para cada subregión válida
typedef struct {
    int     x0, z0;        // esquina inferior izquierda (chunks)
    double  distance;      // dist. euclídea al punto central
} RegionResult;

// API pública
int   search_create(const SearchParams *p, SearchContext **outCtx);
int   search_run   (SearchContext    *ctx,
                    RegionResult   **outResults,
                    int             *outCount);
void  search_destroy(SearchContext    *ctx);
int run_search_pipeline(uint64_t seed,
                        criterioBusqueda *crit,
                        RegionResult **outResults,
                        int *outCount);

#endif /* SEARCH_H */
