
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#include "list.h"
#include "search_cli.h"
#include "criteria.h"
#include "search.h"
#include "extra.h"
#include "kd_tree.h"
#include "finders.h"
#include "util.h"
#include "generator.h"
#include "biomes.h"

#ifndef WINDOW_SIZE
#define WINDOW_SIZE 64
#endif

typedef struct {
    const RegionResult *res;
    double score;
} Scored;

extern int biome_name_to_id(const char *name);
extern int struct_name_to_type(const char *name);

bool cumpleRequisitosBusqueda(criterioBusqueda *c, uint64_t *pseed)
{
    if (list_size(c->biomasRequeridos) == 0 || list_size(c->estructurasRequeridas) == 0 || *pseed == 0) return false;
    return true;
}

int realizarBusqueda(criterioBusqueda* c,
                     uint64_t *pseed,
                     RegionResult **outArr,
                     int *outCount) {
    limpiarPantalla();
    puts("=======================================");
    puts("         Búsqueda de Criterios");
    puts("=======================================");
    if (!cumpleRequisitosBusqueda(c, pseed))
    {
        puts("Debes completar tus criterios y/o tu seed para hacer una búsqueda...");
        printf("%" PRIu64 "\n", *pseed);
        presioneEnterParaContinuar();
        return -1;
    }


    resumenCriterios(c, 0);
    puts("\n==== Semilla de Búsqueda ====");
    printf("%" PRIu64 "\n", *pseed);
    printf("\nConfirme la búsqueda con estos criterios (s/n): ");
    

    fflush(stdout);
    char buffer[32];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        puts("Entrada inválida.");
        presioneEnterParaContinuar();
        return -1;
    }
    // Validar que la entrada sea exactamente "s\n" o "S\n"
    if (strcmp(buffer, "s\n") != 0 && strcmp(buffer, "S\n") != 0)
    {
        puts("Búsqueda cancelada.");
        presioneEnterParaContinuar();
        return -1;
    }

    RegionResult *resArr = NULL;
    int resCount = 0;
    int err = run_search_pipeline(*pseed, c, &resArr, &resCount);
    if (err != 0) {
        puts("Error interno al ejecutar la búsqueda.");
        presioneEnterParaContinuar();
        *outArr = NULL;
        *outCount = 0;
        return -1;
    }

    // 4) Notificar resultado
    if (resCount == 0) {
        puts("\nNo se encontraron zonas que cumplan los criterios.");
        presioneEnterParaContinuar();
        *outArr = NULL;
        *outCount = 0;
        return 0;
    } else {
        printf("\nBúsqueda exitosa: %d zona(s) encontradas.\n", resCount);
        presioneEnterParaContinuar();

        // Entregar los resultados al llamador
        *outArr   = resArr;
        *outCount = resCount;
        return 1;
    }
    return -1;
}

uint64_t generate_random_seed(void) {
    static int initialized = 0;
    if (!initialized) {
        // Semilla inicial para rand(): mezcla el tiempo y la dirección de una variable
        unsigned seed = (unsigned)time(NULL) ^ (unsigned)(uintptr_t)&initialized;
        srand(seed);
        initialized = 1;
    }

    uint64_t r = 0;
    
    for (int i = 0; i < 5; ++i) {
        r = (r << 15) | (rand() & 0x7FFF);
    }
    return r;
}

void menuModSemilla(){
    puts("=======================================");
    puts("   Modificar Semilla de Generación");
    puts("=======================================");
    puts("1. Ingresar Semilla Propia");
    puts("2. Generar Semilla Aleatoria");
    puts("3. Volver al menú de Búsqueda");
}

void guardarSemilla(uint64_t *pseed, int flag) {
    char buf[32];
    if (flag == 1) {
        // El usuario ingresa su propia semilla
        while (1) {

            limpiarPantalla();
            puts("===== Guardar semilla propia =====");
            printf("Ingrese su semilla (solo dígitos, hasta 20 caracteres): ");
            if (!fgets(buf, sizeof(buf), stdin)) {
                puts("Error de entrada. Intente de nuevo.");
                presioneEnterParaContinuar();
                return;
            }
            // Elimina '\n'
            buf[strcspn(buf, "\r\n")] = '\0';
            size_t len = strlen(buf);
            if (len == 0) {
                puts("No ingresó nada. Intente de nuevo.");
                presioneEnterParaContinuar();
                return;
            }
            else if (len > 20) {
                puts("Excedió el máximo de 20 dígitos, intente con otra semilla.");
                presioneEnterParaContinuar();
                return;
            }

            else {
                // Verifica que todos sean dígitos
                int valid = 1;
                for (size_t i = 0; i < len; ++i) {
                    if (!isdigit((unsigned char)buf[i])) {
                        valid = 0;
                        break;
                    }
                }
                if (!valid) {
                    puts("La semilla debe contener solo dígitos, intente de nuevo.");
                    presioneEnterParaContinuar();
                    return;
                } else {
                    // Convierte a uint64_t
                    errno = 0;
                    uint64_t val = strtoull(buf, NULL, 10);
                    if (errno != 0) {
                        puts("Error al convertir la semilla. Intente de nuevo.");
                        presioneEnterParaContinuar();
                        return;
                    } else {
                        *pseed = val;
                        printf("Semilla establecida a: %" PRIu64 "\n", *pseed);
                        presioneEnterParaContinuar();
                        return;
                    }
                }
            }
        }
    }

    else {
        // Genera semilla aleatoria
        limpiarPantalla();
        puts("===== Generar semilla aleatoria =====");
        *pseed = generate_random_seed();
        printf("Semilla aleatoria generada: %" PRIu64 "\n", *pseed);
        presioneEnterParaContinuar();
    }
}

void modificarSemilla(uint64_t *pseed){
    int opcion;
    do
    {
        limpiarPantalla();
        menuModSemilla();
        printf("Seleccione una opción (1-3): ");
        opcion = leerOpcion(1,3);

        switch(opcion)
        {
            case 1:
                guardarSemilla(pseed, 1);
                break;

            case 2:
                guardarSemilla(pseed, 0);
                break;

            case 3:
                puts("Volviendo al menú de búsqueda...");
                presioneEnterParaContinuar();
                return;

            default:
                puts("Opción inválida, intente de nuevo.");
                presioneEnterParaContinuar();
                break;
        }

    } while (1);

}

static int cmpScored(const void *va, const void *vb) {
    const Scored *a = va, *b = vb;
    if (a->score < b->score) return  1;
    if (a->score > b->score) return -1;
    return 0;
}

double compute_region_score(const criterioBusqueda *c,
                            uint64_t seed,
                            const RegionResult *r)
{
    const int M = WINDOW_SIZE;
    int x0 = r->x0, z0 = r->z0;

    // 1) Inicializar generador
    Generator gen;
    setupGenerator(&gen, MC_1_16, 0);
    applySeed(&gen, DIM_OVERWORLD, seed);

    // 2) Generar biomas en el bloque
    Range range = {16, x0, z0, M, M, 0, 1};
    int *cache = allocCache(&gen, range);
    genBiomes(&gen, cache, range);

    // 3) Recoger posiciones de cada criterio
    // Máximo 4 criterios (2 biomas, 2 estructuras)
    typedef struct { double x, z; } Pt;
    Pt pts[4];
    int n = 0;

    // 3a) Biomas
    for (int i = 0; i < list_size(c->biomasRequeridos); ++i) {
        ID *bid = list_index(c->biomasRequeridos, i);
        int bioId = biome_name_to_id(bid->nombreCub);
        bool found = false;
        for (int dz = 0; dz < M && !found; ++dz) {
            for (int dx = 0; dx < M; ++dx) {
                if (cache[dz*M + dx] == bioId) {
                    pts[n++] = (Pt){ .x = x0 + dx, .z = z0 + dz };
                    found = true;
                    break;
                }
            }
        }
    }

    // 3b) Estructuras
    for (int i = 0; i < list_size(c->estructurasRequeridas); ++i) {
        ID *sid = list_index(c->estructurasRequeridas, i);
        int stype = struct_name_to_type(sid->nombreCub);
        bool found = false;
        for (int dz = 0; dz < M && !found; ++dz) {
            for (int dx = 0; dx < M; ++dx) {
                int wx = x0 + dx, wz = z0 + dz;
                if (isViableStructurePos(stype, &gen, wx, wz, DIM_OVERWORLD)) {
                    pts[n++] = (Pt){ .x = wx, .z = wz };
                    found = true;
                    break;
                }
            }
        }
    }

    free(cache);

    // 4) Si nos faltó alguno, penaliza severamente
    if (n < list_size(c->biomasRequeridos) + list_size(c->estructurasRequeridas)) {
        return 0.0;
    }

    // 5) Calcular distancia máxima (diámetro) entre puntos
    double maxd = 0.0;
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            double dx = pts[i].x - pts[j].x;
            double dz = pts[i].z - pts[j].z;
            double d = sqrt(dx*dx + dz*dz);
            if (d > maxd) maxd = d;
        }
    }

    // 6) Score: cuanto menor el diámetro, mayor el score.
    // Escalamos a [0,1]: score = 1 / (1 + maxd)
    return 1.0 / (1.0 + maxd);
}

void mostrarResultados(const criterioBusqueda *c,
                       uint64_t seed,
                       const RegionResult *arr,
                       int count,
                       KDTree *tree)
{
    limpiarPantalla();
    puts("==============================");
    puts("     Mostrar Resultados");
    puts("==============================");

    if (count <= 0) {
        puts("No hay resultados para mostrar. Ejecuta primero la búsqueda.");
        presioneEnterParaContinuar();
        return;
    }

    int toShow = count < 15 ? count : 15;
    Scored tmp[15];
    for (int i = 0; i < toShow; ++i) {
        tmp[i].res   = &arr[i];
        tmp[i].score = compute_region_score(c, seed, &arr[i]);
        double *pd = malloc(sizeof(double));
        *pd = tmp[i].score;
        KDPoint pt = { .x = arr[i].x0, .z = arr[i].z0, .data = pd };
        kd_insert(tree, pt);
    }

    // Usa cmpScored en lugar de lambda
    qsort(tmp, toShow, sizeof(tmp[0]), cmpScored);

    puts("\nResultados ordenados por puntaje (mayor = más cercanos):");
    for (int i = 0; i < toShow; ++i) {
        const RegionResult *r = tmp[i].res;
        printf(" [%2d] x=%5d, z=%5d, score=%.3f\n",
               i+1, r->x0, r->z0, tmp[i].score);
    }
    presioneEnterParaContinuar();
}