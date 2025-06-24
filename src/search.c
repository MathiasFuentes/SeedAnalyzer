#include "search.h"

#include "list.h"
#include "heap.h"
#include "map.h"
#include "kd_tree.h"

#include "criteria.h"
#include "extra.h"

#include "finders.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define DEFAULT_RADIO    1028   // si el usuario no da radio
#define DEFAULT_SUBSIZE    64   // M por defecto

static Map *biome_map = NULL;
static Map *structure_map = NULL;
/*
    Funciones de búsqueda internas del progama
    para realizar las búsquedas de la región.
*/
static void init_type_maps() {
    if (biome_map) return;
    biome_map = map_create(32, NULL, NULL);
    // Biomas
    map_put(biome_map, "mushroom_fields", (void*)(intptr_t)mushroom_fields);
    map_put(biome_map, "plains",           (void*)(intptr_t)plains);
    map_put(biome_map, "forest",           (void*)(intptr_t)forest);
    map_put(biome_map, "birch_forest",     (void*)(intptr_t)birch_forest);
    map_put(biome_map, "desert",           (void*)(intptr_t)desert);
    map_put(biome_map, "taiga",            (void*)(intptr_t)taiga);
    map_put(biome_map, "jungle",           (void*)(intptr_t)jungle);
    map_put(biome_map, "savanna",          (void*)(intptr_t)savanna);
    map_put(biome_map, "swamp",            (void*)(intptr_t)swamp);
    map_put(biome_map, "ocean",            (void*)(intptr_t)ocean);
    map_put(biome_map, "warm_ocean",       (void*)(intptr_t)warm_ocean);
    map_put(biome_map, "frozen_ocean",     (void*)(intptr_t)frozen_ocean);
    map_put(biome_map, "dark_forest",      (void*)(intptr_t)dark_forest);
    map_put(biome_map, "stone_shore",      (void*)(intptr_t)stone_shore);

    structure_map = map_create(16, NULL, NULL);
    // Estructuras
    map_put(structure_map, "village",         (void*)(intptr_t)Village);
    map_put(structure_map, "desert_pyramid",  (void*)(intptr_t)Desert_Pyramid);
    map_put(structure_map, "jungle_pyramid",  (void*)(intptr_t)Jungle_Pyramid);
    map_put(structure_map, "pillager_outpost",(void*)(intptr_t)Outpost);
    map_put(structure_map, "ocean_monument",  (void*)(intptr_t)Monument);
    map_put(structure_map, "ruined_portal",   (void*)(intptr_t)Ruined_Portal);
    map_put(structure_map, "buried_treasure", (void*)(intptr_t)Treasure);
    map_put(structure_map, "mineshaft",       (void*)(intptr_t)Mineshaft);
    map_put(structure_map, "stronghold",      (void*)(intptr_t)(-2));
}

static bool biomeTypeFromString(const char *name, BiomeType *out) {
    init_type_maps();
    void *v = map_get(biome_map, name);
    if (!v) return false;
    *out = (BiomeType)(intptr_t)v;
    return true;
}

static bool structureTypeFromString(const char *name, StructureType *out) {
    init_type_maps();
    void *v = map_get(structure_map, name);
    if (!v) return false;
    *out = (StructureType)(intptr_t)v;
    return true;
}

static inline int rect_sum(const int *ps, int stride, int x0, int z0, int M) {
    int x1 = x0 + M;
    int z1 = z0 + M;
    return ps[z1*stride + x1]
         - ps[z0*stride + x1]
         - ps[z1*stride + x0]
         + ps[z0*stride + x0];
}

SearchContext* search_create(const criterioBusqueda *crit, Generator *gen, uint64_t seed) {
    if (!crit || !gen) return NULL;
    SearchContext *ctx = malloc(sizeof(*ctx));
    if (!ctx) return NULL;

    // Radio y área
    if (list_size(crit->radioBusquedaEnChunks) == 0) ctx->radio = DEFAULT_RADIO;
    else ctx->radio = *list_first(crit->radioBusquedaEnChunks);
    ctx->N = ctx->radio*2 + 1;

    // Coordenadas iniciales
    if (list_size(crit->coordenadasIniciales) < 2) {
        ctx->startX = ctx->startZ = 0.0;
    } else {
        int *xp = list_first(crit->coordenadasIniciales);
        int *zp = list_next(crit->coordenadasIniciales);
        ctx->startX = xp ? *xp : 0.0;
        ctx->startZ = zp ? *zp : 0.0;
    }

    // Subregión y listas
    ctx->M            = DEFAULT_SUBSIZE;
    ctx->biomas       = crit->biomasRequeridos;
    ctx->estructuras  = crit->estructurasRequeridas;
    ctx->nBiomas      = list_size(ctx->biomas);
    ctx->nEstructuras= list_size(ctx->estructuras);

    // Generador
    ctx->gen = gen;
    applySeed(ctx->gen, DIM_OVERWORLD, seed);
    ctx->seed = seed;

    // Reservas
    ctx->biomeId = calloc((size_t)ctx->N*ctx->N, sizeof(int));
    ctx->structFlag = calloc(ctx->nEstructuras, sizeof(bool*));
    for (int i=0; i<ctx->nEstructuras; i++) ctx->structFlag[i] = calloc((size_t)ctx->N*ctx->N, sizeof(bool));
    ctx->prefixBiome  = calloc(ctx->nBiomas, sizeof(int*));
    ctx->prefixStruct = calloc(ctx->nEstructuras, sizeof(int*));
    ctx->candidates     = NULL;
    ctx->candidateCount = 0;
    ctx->heap           = NULL;
    return ctx;
}

void search_destroy(SearchContext *ctx) {
    if (!ctx) return;
    free(ctx->biomeId);
    for (int i=0; i<ctx->nEstructuras; i++) free(ctx->structFlag[i]); free(ctx->structFlag);
    for (int i=0; i<ctx->nBiomas; i++) free(ctx->prefixBiome[i]); free(ctx->prefixBiome);
    for (int i=0; i<ctx->nEstructuras; i++) free(ctx->prefixStruct[i]); free(ctx->prefixStruct);
    free(ctx->candidates);
    if (ctx->heap) heap_destroy(ctx->heap);
    free(ctx);
}

void search_precompute_biomes(SearchContext *ctx) {
    int N = ctx->N; Range r = { .scale=16, .x=(int)ctx->startX-ctx->radio,
        .z=(int)ctx->startZ-ctx->radio, .sx=N, .sz=N, .y=15, .sy=1 };
    int *cache = allocCache(ctx->gen,r);
    genBiomes(ctx->gen, cache, r);
    for (int i=0; i<N*N; i++) ctx->biomeId[i] = cache[i];
    free(cache);
}

void search_precompute_structures(SearchContext *ctx) {
    int N=ctx->N, half=ctx->radio;
    int minX=((int)ctx->startX-half)>>5, maxX=((int)ctx->startX+half)>>5;
    int minZ=((int)ctx->startZ-half)>>5, maxZ=((int)ctx->startZ+half)>>5;
    for(int i=0;i<ctx->nEstructuras;i++){
        memset(ctx->structFlag[i],0,N*N*sizeof(bool));
        ID *sel=list_index(ctx->estructuras,i);
        StructureType type; if(!structureTypeFromString(sel->nombreCub,&type)) continue;
        if(type==(StructureType)-2){
            StrongholdIter sh; Pos p=initFirstStronghold(&sh,MC_1_16,ctx->seed);
            while(p.x||p.z){int rx=(p.x>>4)-((int)ctx->startX-half), rz=(p.z>>4)-((int)ctx->startZ-half);
                if(rx>=0&&rx<N&&rz>=0&&rz<N)ctx->structFlag[i][rz*N+rx]=true;
                if(nextStronghold(&sh,ctx->gen)<=0)break; p=sh.pos;}
            continue;
        }
        for(int rz=minZ;rz<=maxZ;rz++) for(int rx=minX;rx<=maxX;rx++){
            Pos pos; if(!getStructurePos(type,MC_1_16,ctx->seed&((1ULL<<48)-1),rx,rz,&pos)) continue;
            int ix=(pos.x>>4)-((int)ctx->startX-half), iz=(pos.z>>4)-((int)ctx->startZ-half);
            if(ix>=0&&ix<N&&iz>=0&&iz<N&&isViableStructurePos(type,ctx->gen,pos.x>>4,pos.z>>4,0))
                ctx->structFlag[i][iz*N+ix]=true;
        }
    }
}

void search_build_prefix_sums(SearchContext *ctx){
    int N=ctx->N, stride=N+1;
    for(int b=0;b<ctx->nBiomas;b++){int* ps=calloc((N+1)*(N+1),sizeof(int));ctx->prefixBiome[b]=ps;
        BiomeType want; list_index(ctx->biomas,b); want=list_index(ctx->biomas,b)->id;
        for(int y=0;y<N;y++) for(int x=0;x<N;x++){
            int add = (ctx->biomeId[y*N+x]==want);
            int idx=(y+1)*stride+(x+1);
            ps[idx]=ps[idx-1]+ps[idx-stride]-ps[idx-stride-1]+add;
        }}
    for(int s=0;s<ctx->nEstructuras;s++){int* ps=calloc((N+1)*(N+1),sizeof(int));ctx->prefixStruct[s]=ps;
        for(int y=0;y<N;y++) for(int x=0;x<N;x++){
            int add=ctx->structFlag[s][y*N+x];
            int idx=(y+1)*stride+(x+1);
            ps[idx]=ps[idx-1]+ps[idx-stride]-ps[idx-stride-1]+add;
        }}
}

bool search_find_candidate_region(SearchContext *ctx, Region *out) {
    if(!ctx||!out) return false;
    // Lista de regiones candidatas
    int N=ctx->N, M=ctx->M, half=ctx->radio;
    // Construir KDTree
    KDTree* tree=kd_create(N*N);
    // Insertar solo candidatos
    for(int dz=0;dz<=N-M;dz++)for(int dx=0;dx<=N-M;dx++){
        bool ok=true;
        for(int b=0;b<ctx->nBiomas;b++) if(rect_sum(ctx->prefixBiome[b],N+1,dx,dz,M)==0){ok=false;break;}
        if(!ok) continue;
        for(int s=0;s<ctx->nEstructuras;s++) if(rect_sum(ctx->prefixStruct[s],N+1,dx,dz,M)==0){ok=false;break;}
        if(!ok) continue;
        Region r=make_region(ctx,dx,dz);
        KDPoint p={r.centerX,r.centerZ, malloc(sizeof(Region))};
        *(Region*)p.data = r;
        kd_insert(tree,p);
    }
    // Buscar el más cercano a la semilla
    KDPoint seed={ctx->startX,ctx->startZ,NULL}, nearest;
    if(!kd_nearest(tree,seed,&nearest)){kd_destroy(tree,true);return false;}
    // Devolver región
    Region best = *(Region*)nearest.data;
    *out = best;
    // Limpieza
    kd_destroy(tree,true);
    return true;
}

void run_search_pipeline(const criterioBusqueda *crit, Generator *gen, uint64_t seed) {
    SearchContext* ctx = search_create(crit,gen,seed);
    if(!ctx) return;
    search_precompute_biomes(ctx);
    search_precompute_structures(ctx);
    search_build_prefix_sums(ctx);
    Region zona;
    if(search_find_candidate_region(ctx,&zona)){
        printf("Zona elegida: x0=%d, z0=%d, centro=(%.1f,%.1f), dist=%.2f\n",
               zona.x0,zona.z0,zona.centerX,zona.centerZ,zona.distance);
    } else printf("No se encontró ninguna región que cumpla los criterios.\n");
    search_destroy(ctx);
}


/*
    Funciones de interacción con el usuario
*/

void realizarBusqueda(criterioBusqueda* c, Generator *gen, uint64_t seed){
    
    limpiarPantalla();
    puts("===============================================");
    puts("  Búsqueda del terreno");
    puts("===============================================");;
    
    if (seed == 0)
    {
        puts("");
    }
}