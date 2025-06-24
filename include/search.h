#ifndef SEARCH_H
#define SEARCH_H

#include "criteria.h"
#include "generator.h"

// -----------------------------------------------------------------------------
// Representa una subregión candidata de tamaño M×M en coordenadas X,Z
// -----------------------------------------------------------------------------
typedef struct {
    int x0, z0;           // esquina inferior-izquierda de la subregión
    double centerX;       // coordenada X del centro: x0 + M/2
    double centerZ;       // coordenada Z del centro: z0 + M/2
    double distance;      // distancia euclídea al punto inicial (X_init,Z_init)
} Region;

// -----------------------------------------------------------------------------
// Contexto de búsqueda: encapsula todos los datos intermedios y TDAs
// -----------------------------------------------------------------------------
typedef struct SearchContext SearchContext;

// -----------------------------------------------------------------------------
// 1. Crear y destruir el contexto de búsqueda
// -----------------------------------------------------------------------------

/**
 * Crea un SearchContext a partir de los criterios del usuario y un Generator
 * ya inicializado (Cubiomes).
 *
 * - Reserva memoria para matrices N×N, prefix sums, lista de candidatas, heap...
 * - Aplica la seed y dimensión en el Generator.
 *
 * Complejidad: O(1)
 */
SearchContext* search_create(const criterioBusqueda *criteria, Generator *gen, uint64_t seed);

/**
 * Libera todos los recursos asociados al SearchContext.
 */
void search_destroy(SearchContext *ctx);

// -----------------------------------------------------------------------------
// 2. Precomputación de biomas y estructuras
// -----------------------------------------------------------------------------

/**
 * Simula la generación de biomas en todo el área N×N de chunks usando Cubiomes.
 * Rellena una matriz biomeId[N][N].
 *
 * Complejidad: O(N²)
 */
void search_precompute_biomes(SearchContext *ctx);

/**
 * Para cada estructura requerida:
 *  - Marca en una matriz structFlag[k][N][N] si la estructura aparece en ese chunk.
 *
 * Complejidad aproximada: O(N²) amortizado
 */
void search_precompute_structures(SearchContext *ctx);

/**
 * Construye las matrices de prefix sums (sumas acumuladas) para:
 *  - Cada bioma requerido (≤3)
 *  - Cada estructura requerida (≤3)
 *
 * Permite consultar en O(1) el conteo dentro de cualquier subcuadrado.
 *
 * Complejidad: O((b + s) · N²), con b = #biomas, s = #estructuras
 */
void search_build_prefix_sums(SearchContext *ctx);

// -----------------------------------------------------------------------------
// 3. Detección de regiones candidatas
// -----------------------------------------------------------------------------

/**
 * Recorre todas las subregiones de tamaño M×M (R ≈ (N−M+1)²) y, para cada una:
 *  - Usa prefix sums para verificar en O(1) que cumple todos los criterios
 *    (biomas y estructuras).
 *  - Almacena las que cumplen en una lista/vectores de Region.
 *
 * Complejidad: O(R)
 */
void search_find_candidate_regions(SearchContext *ctx);

// -----------------------------------------------------------------------------
// 4. Ranking de regiones por distancia
// -----------------------------------------------------------------------------

/**
 * Rankea las regiones candidatas según la distancia de su centro al punto
 * inicial (X_init, Z_init) usando un min-heap de tamaño top_k.
 *
 * - Si top_k == 1, es O(R).
 * - Para top_k > 1, O(R · log top_k).
 *
 * Alternativamente, para obtener todas ordenadas, puede usarse O(R·log R) con qsort.
 */
void search_rank_regions(SearchContext *ctx, int top_k);

// -----------------------------------------------------------------------------
// 5. Recuperar resultados
// -----------------------------------------------------------------------------

/**
 * Devuelve un puntero a la mejor región (top 1). NULL si no hay candidatas.
 */
const Region* search_get_best_region(const SearchContext *ctx);

/**
 * Devuelve un arreglo (inmutable) de las top_k regiones ordenadas y escribe
 * en *out_count el número real de regiones devueltas (≤ top_k).
 */
const Region* search_get_top_k(const SearchContext *ctx, int *out_count);

/**
 * Devuelve todas las regiones candidatas sin ordenar y escribe el total en
 * *out_count. Útil para debug o ranking personalizado.
 */
const Region* search_get_all_regions(const SearchContext *ctx, int *out_count);

void run_search_pipeline(const criterioBusqueda *crit, Generator *gen, uint64_t seed);

#endif // SEARCH_H
