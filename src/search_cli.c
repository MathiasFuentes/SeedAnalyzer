
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "list.h"
#include "search_cli.h"
#include "criteria.h"
#include "search.h"
#include "extra.h"

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