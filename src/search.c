#include "search.h"
#include "criteria.h"
#include "generator.h"
#include "util.h"
#include "biomes.h"
#include "finders.h"
#include "list.h"
#include "quadbase.h"
#include "extra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>


#define WINDOW_SIZE 64
#define MAX_DISPLAY 5
#define MIN_SEPARATION 16.0

static int cmpRegion(const void *a, const void *b) {
    double da = ((const RegionResult*)a)->distance;
    double db = ((const RegionResult*)b)->distance;
    return (da < db) ? -1 : (da > db);
}

int biome_name_to_id(const char *name) {
    if (strcmp(name, "plains") == 0)        return plains;
    if (strcmp(name, "forest") == 0)        return forest;
    if (strcmp(name, "birch_forest") == 0)  return birch_forest;
    if (strcmp(name, "desert") == 0)        return desert;
    if (strcmp(name, "taiga") == 0)         return taiga;
    if (strcmp(name, "jungle") == 0)        return jungle;
    if (strcmp(name, "savanna") == 0)       return savanna;
    if (strcmp(name, "swamp") == 0)         return swamp;
    if (strcmp(name, "ocean") == 0)         return ocean;
    if (strcmp(name, "dark_forest") == 0)   return dark_forest;
    // Si más biomas se agregan a Biomas[], mapea aquí...
    return -1;
}

static int struct_name_to_type(const char *name) {
    if (strcmp(name, "village") == 0)           return Village;
    if (strcmp(name, "desert_pyramid") == 0)    return Desert_Pyramid;
    if (strcmp(name, "jungle_pyramid") == 0)    return Jungle_Pyramid;
    if (strcmp(name, "pillager_outpost") == 0)  return Outpost;
    if (strcmp(name, "ocean_monument") == 0)    return Monument;
    if (strcmp(name, "stronghold") == 0)        return Fortress;
    if (strcmp(name, "ruined_portal") == 0)     return Ruined_Portal;
    if (strcmp(name, "buried_treasure") == 0)   return Treasure;
    if (strcmp(name, "Mineshaft") == 0)         return Mineshaft;
    // Si más estructuras se agregan al catálogo, mapea aquí...
    
    return -1;
}

// Helper: count bits in a mask
static int count_bits(uint64_t mask) {
    int cnt = 0;
    while (mask) {
        cnt += (mask & 1ULL);
        mask >>= 1;
    }
    return cnt;
}

// Helper: fill array with IDs of set bits in mask
static void fill_ids(uint64_t mask, int *ids, int count) {
    int idx = 0;
    for (int i = 0; i < 64 && idx < count; ++i) {
        if (mask & (1ULL << i)) {
            ids[idx++] = i;
        }
    }
}

// Build bitmask from user-selected IDs (using list_index)
static uint64_t mask_from_list(List *lst, ID *catalog, int total) {
    uint64_t mask = 0;
    int sz = list_size(lst);
    for (int i = 0; i < sz; ++i) {
        ID *item = (ID*)list_index(lst, i);
        if (!item) continue;
        for (int j = 0; j < total; ++j) {
            if (strcmp(item->nombreCub, catalog[j].nombreCub) == 0) {
                mask |= (1ULL << j);
                break;
            }
        }
    }
    return mask;
}

// Wrapper: create SearchContext directly from criterioBusqueda
int search_create_from_criterio(const criterioBusqueda *crit,
                                Generator *gen,
                                uint64_t seed,
                                int mcVersion,
                                int dim,
                                int subSize,
                                int maxThreads,
                                SearchContext **outCtx) {
    if (!crit || !gen || !outCtx) return -1;
    SearchParams p = {0};
    p.gen = gen;
    p.seed = seed;
    p.mcVersion = mcVersion;
    p.dim = dim;
    int radius = list_size(crit->radioBusquedaEnChunks)
        ? *(int*)list_index(crit->radioBusquedaEnChunks, 0)
        : 1028;
    p.totalSize = radius * 2 + 1;
    if (list_size(crit->coordenadasIniciales)) {
        int *coords = (int*)list_index(crit->coordenadasIniciales, 0);
        p.startX = coords[0];
        p.startZ = coords[1];
    } else {
        p.startX = 0;
        p.startZ = 0;
    }
    p.subSize = subSize;
    p.maxThreads = maxThreads;
    p.biomeMask = mask_from_list(crit->biomasRequeridos, Biomas, TOTAL_BIOMAS);
    p.structureMask = mask_from_list(crit->estructurasRequeridas, Estructuras, TOTAL_ESTRUCTURAS);
    return search_create(&p, outCtx);
}

// Generate full N×N biome grid via Cubiomes
static int generate_biome_grid(SearchContext *ctx) {
    int N = ctx->params.totalSize;
    Range r = {16,
               ctx->params.startX - N/2,
               ctx->params.startZ - N/2,
               N, N,
               0, 1};
    int *cache = allocCache(ctx->params.gen, r);
    if (!cache) return -1;
    genBiomes(ctx->params.gen, cache, r);
    memcpy(ctx->biomeGrid, cache, sizeof(int) * N * N);
    free(cache);
    return 0;
}

// Detect structures over the N×N area
static void detect_structures(SearchContext *ctx) {
    int N     = ctx->params.totalSize;
    int baseX = ctx->params.startX - N/2;
    int baseZ = ctx->params.startZ - N/2;

    for (int s = 0; s < ctx->numStructures; ++s) {
        int type = ctx->structIds[s];  // ya es el enum real tras el mapeo en search_create
        bool *grid = ctx->structGrid + (size_t)s * N * N;
        memset(grid, 0, sizeof(bool) * N * N);

        for (int x = 0; x < N; ++x) {
            for (int z = 0; z < N; ++z) {
                int wx = baseX + x;
                int wz = baseZ + z;
                // Sólo comprobamos isViableStructurePos en el chunk
                if (isViableStructurePos(type, ctx->params.gen, wx, wz, ctx->params.dim)) {
                    grid[x * N + z] = true;
                }
            }
        }
    }
}

// Build prefix-sum tables for fast area queries
static void build_prefix_sums(SearchContext *ctx) {
    int N = ctx->params.totalSize;
    int P = N + 1;
    for (int b = 0; b < ctx->numBiomes; ++b) {
        int id = ctx->biomeIds[b];
        int *pref = ctx->prefixBiome[b];
        memset(pref, 0, sizeof(int) * P * P);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                int val = (ctx->biomeGrid[i * N + j] == id) ? 1 : 0;
                pref[(i+1)*P + (j+1)] = val
                    + pref[i*P + (j+1)]
                    + pref[(i+1)*P + j]
                    - pref[i*P + j];
            }
        }
    }
    for (int s = 0; s < ctx->numStructures; ++s) {
        int *pref = ctx->prefixStruct[s];
        bool *grid = ctx->structGrid + (size_t)s * N * N;
        memset(pref, 0, sizeof(int) * P * P);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                int val = grid[i * N + j] ? 1 : 0;
                pref[(i+1)*P + (j+1)] = val
                    + pref[i*P + (j+1)]
                    + pref[(i+1)*P + j]
                    - pref[i*P + j];
            }
        }
    }
}

// Slide M×M window, check criteria, collect results
static void scan_structures_first(SearchContext *ctx,
                                  RegionResult **out,
                                  int *outCount)
{
    int N     = ctx->params.totalSize;
    int M     = ctx->params.subSize;     // 64
    int halfM = M/2;
    int baseX = ctx->params.startX - N/2;
    int baseZ = ctx->params.startZ - N/2;
    int B     = ctx->numBiomes;
    int S     = ctx->numStructures;

    // 1) Encuentra la estructura menos frecuente
    int s_min = 0, min_cnt = N*N+1;
    for (int s = 0; s < S; ++s) {
        bool *g = ctx->structGrid + (size_t)s * N * N;
        int cnt = 0;
        for (int i = 0; i < N*N; ++i) cnt += g[i];
        if (cnt < min_cnt) { min_cnt = cnt; s_min = s; }
    }
    bool *minGrid = ctx->structGrid + (size_t)s_min * N * N;

    int cap = 16;
    RegionResult *res = malloc(sizeof(*res) * cap);
    int cntRes = 0;
    int P = N + 1;

    // 2) Itera sólo sobre chunks con esa estructura
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (!minGrid[i * N + j]) continue;
            int x0 = i - halfM, z0 = j - halfM;
            if (x0 < 0) x0 = 0;
            if (z0 < 0) z0 = 0;
            if (x0 + M > N) x0 = N - M;
            if (z0 + M > N) z0 = N - M;

            // 3) Comprueba **todos** los biomas requeridos con prefijos
            bool ok = true;
            for (int b = 0; b < B; ++b) {
                int *pb = ctx->prefixBiome[b];
                int sum = pb[(x0+M)*P + (z0+M)]
                        - pb[x0*P     + (z0+M)]
                        - pb[(x0+M)*P +  z0   ]
                        + pb[x0*P     +  z0   ];
                if (sum == 0) { ok = false; break; }
            }
            if (!ok) continue;

            // 4) Comprueba **todas** las demás estructuras con prefijos
            for (int s = 0; s < S; ++s) {
                if (s == s_min) continue;
                int *ps = ctx->prefixStruct[s];
                int sum = ps[(x0+M)*P + (z0+M)]
                        - ps[x0*P     + (z0+M)]
                        - ps[(x0+M)*P +  z0   ]
                        + ps[x0*P     +  z0   ];
                if (sum == 0) { ok = false; break; }
            }
            if (!ok) continue;

            // 5) Guarda sin duplicados
            int realX = baseX + x0, realZ = baseZ + z0;
            bool dup = false;
            for (int k = 0; k < cntRes; ++k) {
                if (res[k].x0 == realX && res[k].z0 == realZ) {
                    dup = true; break;
                }
            }
            if (dup) continue;

            if (cntRes >= cap) {
                cap *= 2;
                res = realloc(res, sizeof(*res) * cap);
            }
            res[cntRes].x0       = realX;
            res[cntRes].z0       = realZ;
            double dx            = realX - ctx->params.startX;
            double dz            = realZ - ctx->params.startZ;
            res[cntRes].distance = sqrt(dx*dx + dz*dz);
            cntRes++;
        }
    }

    *out      = res;
    *outCount = cntRes;
}

// Public API: allocate context and buffers
int search_create(const SearchParams *p, SearchContext **outCtx) {
    if (!p || !outCtx) return -1;

    SearchContext *ctx = malloc(sizeof(SearchContext));
    if (!ctx) return -1;
    ctx->params = *p;
    int N = p->totalSize;

    // 1) Decode biomeMask → biomeIds[]
    ctx->numBiomes = count_bits(p->biomeMask);
    ctx->biomeIds  = malloc(sizeof(int) * ctx->numBiomes);
    fill_ids(p->biomeMask, ctx->biomeIds, ctx->numBiomes);
    for (int k = 0; k < ctx->numBiomes; ++k) {
        int catalogIdx = ctx->biomeIds[k];               // índice en Biomas[]
        int realId     = biome_name_to_id(Biomas[catalogIdx].nombreCub);
        if (realId < 0) {
            fprintf(stderr,
                    "Error: bioma desconocido '%s'\n",
                    Biomas[catalogIdx].nombreCub);
        }
        ctx->biomeIds[k] = realId;                       // ahora contiene el ID real
    }

    // 2) Decode structureMask → structIds[] (catalog indices)
    ctx->numStructures = count_bits(p->structureMask);
    ctx->structIds     = malloc(sizeof(int) * ctx->numStructures);
    fill_ids(p->structureMask, ctx->structIds, ctx->numStructures);

    // 3) Map catalog indices → real enum StructureType values
    for (int i = 0; i < ctx->numStructures; ++i) {
        int catalogIdx = ctx->structIds[i];  // index into Estructuras[]
        int type = struct_name_to_type(Estructuras[catalogIdx].nombreCub);
        if (type < 0) {
            fprintf(stderr,
                    "Error: estructura desconocida '%s' (índice %d)\n",
                    Estructuras[catalogIdx].nombreCub,
                    catalogIdx);
        }
        ctx->structIds[i] = type;  // now holds the enum value
    }

    // 4) Allocate grids and prefix tables
    ctx->biomeGrid  = malloc(sizeof(int)  * N * N);
    ctx->structGrid = malloc(sizeof(bool) * ctx->numStructures * N * N);

    int P = N + 1;
    ctx->prefixBiome  = malloc(sizeof(int*) * ctx->numBiomes);
    for (int b = 0; b < ctx->numBiomes; ++b) {
        ctx->prefixBiome[b] = malloc(sizeof(int) * P * P);
    }
    ctx->prefixStruct = malloc(sizeof(int*) * ctx->numStructures);
    for (int s = 0; s < ctx->numStructures; ++s) {
        ctx->prefixStruct[s] = malloc(sizeof(int) * P * P);
    }

    *outCtx = ctx;
    return 0;
}

// Run full search pipeline and return results
int search_run(SearchContext *ctx, RegionResult **outResults, int *outCount) {
    if (!ctx || !outResults || !outCount) return -1;
    if (generate_biome_grid(ctx) != 0) return -1;
    detect_structures(ctx);
    build_prefix_sums(ctx);
    scan_structures_first(ctx, outResults, outCount);
    return 0;
}

// Free all allocated memory
void search_destroy(SearchContext *ctx) {
    if (!ctx) return;
    //int N = ctx->params.totalSize;
    for (int b = 0; b < ctx->numBiomes; ++b) free(ctx->prefixBiome[b]);
    free(ctx->prefixBiome);
    for (int s = 0; s < ctx->numStructures; ++s) free(ctx->prefixStruct[s]);
    free(ctx->prefixStruct);
    free(ctx->biomeIds);
    free(ctx->structIds);
    free(ctx->biomeGrid);
    free(ctx->structGrid);
    free(ctx);
}

static bool block_has_biomes(Generator *gen,
                             int cornerX, int cornerZ,
                             int *biomeIds, int numBiomes)
{
    const int M = WINDOW_SIZE;
    Range r = { 16, cornerX, cornerZ, M, M, 0, 1 };
    int *cache = allocCache(gen, r);
    genBiomes(gen, cache, r);

    for (int b = 0; b < numBiomes; ++b) {
        int id = biomeIds[b];
        bool found = false;
        for (int i = 0; i < M * M; ++i) {
            if (cache[i] == id) { found = true; break; }
        }
        if (!found) {
            free(cache);
            return false;
        }
    }
    free(cache);
    return true;
}

// Test that in the WINDOW_SIZE×WINDOW_SIZE block at (cornerX, cornerZ)
// there is at least one of each requested structure (besides the one we inverted on).
static bool block_has_structures(SearchContext *ctx,
                                 int cornerX, int cornerZ,
                                 int skipStructIndex)
{
    const int M = WINDOW_SIZE;
    for (int s = 0; s < ctx->numStructures; ++s) {
        if (s == skipStructIndex) continue;
        int type = ctx->structIds[s];
        bool found = false;
        for (int dx = 0; dx < M && !found; ++dx) {
            for (int dz = 0; dz < M; ++dz) {
                int wx = cornerX + dx;
                int wz = cornerZ + dz;
                if (isViableStructurePos(type, ctx->params.gen, wx, wz, ctx->params.dim)) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) return false;
    }
    return true;
}

/*
void run_search_pipeline(uint64_t seed, criterioBusqueda *crit) {
    // 1) Leer criterios de usuario
    int radius = list_size(crit->radioBusquedaEnChunks)
               ? *(int*)list_index(crit->radioBusquedaEnChunks, 0)
               : 1028;
    int centerX = list_size(crit->coordenadasIniciales)
               ? ((int*)list_index(crit->coordenadasIniciales, 0))[0]
               : 0;
    int centerZ = list_size(crit->coordenadasIniciales)
               ? ((int*)list_index(crit->coordenadasIniciales, 0))[1]
               : 0;

    // 2) Inicializar Generator
    Generator *gen = malloc(sizeof(*gen));
    setupGenerator(gen, MC_1_16, 0);
    applySeed(gen, DIM_OVERWORLD, seed);

    // 3) Crear contexto para decodificar máscaras
    SearchContext *ctx = NULL;
    search_create_from_criterio(
        crit, gen, seed,
        MC_1_16, DIM_OVERWORLD,
        WINDOW_SIZE, 1,
        &ctx
    );

    // 4) Seleccionar la estructura más rara (menos regiones viables)
    int s_min = 0;
    int minCount = INT_MAX;
    for (int s = 0; s < ctx->numStructures; ++s) {
        StructureConfig sc;
        getStructureConfig(ctx->structIds[s],
                           ctx->params.mcVersion,
                           &sc);
        int regSize = sc.regionSize;
        int minRX = (centerX - radius) / regSize;
        int maxRX = (centerX + radius) / regSize;
        int minRZ = (centerZ - radius) / regSize;
        int maxRZ = (centerZ + radius) / regSize;
        int cnt = 0;
        for (int rx = minRX; rx <= maxRX; ++rx) {
            for (int rz = minRZ; rz <= maxRZ; ++rz) {
                Pos p;
                if (!getStructurePos(ctx->structIds[s],
                                     ctx->params.mcVersion,
                                     seed, rx, rz, &p))
                    continue;
                if (isViableStructurePos(ctx->structIds[s],
                                         gen, p.x, p.z,
                                         ctx->params.dim))
                    cnt++;
            }
        }
        if (cnt < minCount) {
            minCount = cnt;
            s_min = s;
        }
    }

    // 5) Enumerar candidatos de la estructura s_min
    StructureConfig sc_min;
    getStructureConfig(ctx->structIds[s_min],
                       ctx->params.mcVersion,
                       &sc_min);
    int regSize = sc_min.regionSize;
    int minRX = (centerX - radius) / regSize;
    int maxRX = (centerX + radius) / regSize;
    int minRZ = (centerZ - radius) / regSize;
    int maxRZ = (centerZ + radius) / regSize;

    Pos *cands = malloc(sizeof(Pos) * (maxRX-minRX+1)*(maxRZ-minRZ+1));
    int nCands = 0;
    for (int rx = minRX; rx <= maxRX; ++rx) {
        for (int rz = minRZ; rz <= maxRZ; ++rz) {
            Pos p;
            if (!getStructurePos(ctx->structIds[s_min],
                                 ctx->params.mcVersion,
                                 seed, rx, rz, &p))
                continue;
            int cx = p.x >> 4;
            int cz = p.z >> 4;
            double d = hypot(cx - centerX, cz - centerZ);
            if (d > radius) continue;
            if (isViableStructurePos(ctx->structIds[s_min],
                                     gen, p.x, p.z,
                                     ctx->params.dim))
            {
                cands[nCands++] = (Pos){ cx, cz };
            }
        }
    }

    // 6) Para cada candidato, probar bloque 64×64
    RegionResult *results = malloc(sizeof(RegionResult) * nCands);
    int foundCount = 0;
    int half = WINDOW_SIZE / 2;
    for (int i = 0; i < nCands; ++i) {
        int cx = cands[i].x;
        int cz = cands[i].z;
        int cornerX = cx - half;
        int cornerZ = cz - half;
        // asegurarse dentro del radio total N = 2*radius+1
        int N = ctx->params.totalSize;
        int baseX = centerX - N/2;
        int baseZ = centerZ - N/2;
        if (cornerX < baseX) cornerX = baseX;
        if (cornerZ < baseZ) cornerZ = baseZ;
        if (cornerX + WINDOW_SIZE > baseX + N)
            cornerX = baseX + N - WINDOW_SIZE;
        if (cornerZ + WINDOW_SIZE > baseZ + N)
            cornerZ = baseZ + N - WINDOW_SIZE;

        // 6a) Biomas
        if (!block_has_biomes(gen, cornerX, cornerZ,
                              ctx->biomeIds,
                              ctx->numBiomes))
            continue;
        // 6b) Otras estructuras
        if (!block_has_structures(ctx, cornerX, cornerZ, s_min))
            continue;

        // 7) Guardar resultado
        double dist = hypot(cornerX + half - centerX,
                            cornerZ + half - centerZ);
        results[foundCount++] = (RegionResult){
            .x0       = cornerX,
            .z0       = cornerZ,
            .distance = dist
        };
    }

    // 8) Imprimir
    printf("Zonas %dx%d encontradas: %d\n",
           WINDOW_SIZE, WINDOW_SIZE, foundCount);

    for (int i = 0; i < foundCount; ++i) {
        if (i == 15) {break;}
        printf(" [%d] x=%d, z=%d, dist=%.1f\n",
               i+1,
               results[i].x0,
               results[i].z0,
               results[i].distance);
    }
    presioneEnterParaContinuar();
    // 9) Liberar memoria
    free(cands);
    free(results);
    search_destroy(ctx);
    free(gen);
}
*/

int run_search_pipeline(uint64_t seed,
                        criterioBusqueda *crit,
                        RegionResult **outResults,
                        int *outCount)
{
    if (!crit || !outResults || !outCount) return -1;

    // 1) Leer criterios de usuario
    int radius = list_size(crit->radioBusquedaEnChunks)
               ? *(int*)list_index(crit->radioBusquedaEnChunks, 0)
               : 1028;
    int centerX = list_size(crit->coordenadasIniciales)
               ? ((int*)list_index(crit->coordenadasIniciales, 0))[0]
               : 0;
    int centerZ = list_size(crit->coordenadasIniciales)
               ? ((int*)list_index(crit->coordenadasIniciales, 0))[1]
               : 0;

    // 2) Inicializar Generator
    Generator *gen = malloc(sizeof(*gen));
    if (!gen) return -1;
    setupGenerator(gen, MC_1_16, 0);
    applySeed(gen, DIM_OVERWORLD, seed);

    // 3) Crear contexto
    SearchContext *ctx = NULL;
    if (search_create_from_criterio(
            crit, gen, seed,
            MC_1_16, DIM_OVERWORLD,
            WINDOW_SIZE, 1,
            &ctx) != 0)
    {
        free(gen);
        return -1;
    }

    // 4) Seleccionar estructura pivote
    int s_min = 0, minCount = INT_MAX;
    for (int s = 0; s < ctx->numStructures; ++s) {
        StructureConfig sc;
        getStructureConfig(ctx->structIds[s],
                           ctx->params.mcVersion,
                           &sc);
        int regSize = sc.regionSize;
        int minRX = (centerX - radius) / regSize;
        int maxRX = (centerX + radius) / regSize;
        int minRZ = (centerZ - radius) / regSize;
        int maxRZ = (centerZ + radius) / regSize;
        int cnt = 0;
        for (int rx = minRX; rx <= maxRX; ++rx) {
            for (int rz = minRZ; rz <= maxRZ; ++rz) {
                Pos p;
                if (!getStructurePos(ctx->structIds[s],
                                     ctx->params.mcVersion,
                                     seed, rx, rz, &p))
                    continue;
                if (isViableStructurePos(ctx->structIds[s],
                                         gen, p.x, p.z,
                                         ctx->params.dim))
                    cnt++;
            }
        }
        if (cnt < minCount) {
            minCount = cnt;
            s_min = s;
        }
    }

    // 5) Enumerar candidatos del pivote
    StructureConfig sc_min;
    getStructureConfig(ctx->structIds[s_min],
                       ctx->params.mcVersion,
                       &sc_min);
    int regSize = sc_min.regionSize;
    int minRX = (centerX - radius) / regSize;
    int maxRX = (centerX + radius) / regSize;
    int minRZ = (centerZ - radius) / regSize;
    int maxRZ = (centerZ + radius) / regSize;

    Pos *cands = malloc(sizeof(Pos) * ((maxRX-minRX+1)*(maxRZ-minRZ+1)));
    if (!cands) { search_destroy(ctx); free(gen); return -1; }
    int nCands = 0;
    for (int rx = minRX; rx <= maxRX; ++rx) {
        for (int rz = minRZ; rz <= maxRZ; ++rz) {
            Pos p;
            if (!getStructurePos(ctx->structIds[s_min],
                                 ctx->params.mcVersion,
                                 seed, rx, rz, &p))
                continue;
            int cx = p.x >> 4;
            int cz = p.z >> 4;
            double d = hypot(cx - centerX, cz - centerZ);
            if (d > radius) continue;
            if (isViableStructurePos(ctx->structIds[s_min],
                                     gen, p.x, p.z,
                                     ctx->params.dim)) {
                cands[nCands++] = (Pos){ cx, cz };
            }
        }
    }

    // 6) Probar bloques y recolectar resultados
    RegionResult *results = malloc(sizeof(RegionResult) * nCands);
    if (!results) { free(cands); search_destroy(ctx); free(gen); return -1; }
    int foundCount = 0;
    int half = WINDOW_SIZE / 2;
    int N = ctx->params.totalSize;
    int baseX = centerX - N/2;
    int baseZ = centerZ - N/2;

    for (int i = 0; i < nCands; ++i) {
        int cx = cands[i].x, cz = cands[i].z;
        int cornerX = cx - half, cornerZ = cz - half;
        if (cornerX < baseX) cornerX = baseX;
        if (cornerZ < baseZ) cornerZ = baseZ;
        if (cornerX + WINDOW_SIZE > baseX + N)
            cornerX = baseX + N - WINDOW_SIZE;
        if (cornerZ + WINDOW_SIZE > baseZ + N)
            cornerZ = baseZ + N - WINDOW_SIZE;

        if (!block_has_biomes(gen, cornerX, cornerZ,
                              ctx->biomeIds, ctx->numBiomes))
            continue;
        if (!block_has_structures(ctx, cornerX, cornerZ, s_min))
            continue;

        double dist = hypot(cornerX + half - centerX,
                            cornerZ + half - centerZ);
        results[foundCount++] = (RegionResult){
            .x0       = cornerX,
            .z0       = cornerZ,
            .distance = dist
        };
    }

    // 7) Ordenar y filtrar separación mínima
    qsort(results, foundCount, sizeof(RegionResult), cmpRegion);
    RegionResult *filtered = malloc(sizeof(RegionResult) * foundCount);
    int fcount = 0;
    for (int i = 0; i < foundCount; ++i) {
        int xi = results[i].x0, zi = results[i].z0;
        bool keep = true;
        for (int j = 0; j < fcount; ++j) {
            double dx = xi - filtered[j].x0;
            double dz = zi - filtered[j].z0;
            if (sqrt(dx*dx + dz*dz) < MIN_SEPARATION) {
                keep = false;
                break;
            }
        }
        if (keep) filtered[fcount++] = results[i];
    }
    free(results);
    free(cands);
    search_destroy(ctx);
    free(gen);

    // 8) Devolver
    *outResults = filtered;
    *outCount   = fcount;
    return 0;
}