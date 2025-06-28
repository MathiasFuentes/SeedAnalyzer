#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "extra.h"
#include "criteria.h"
#include "cargacriterios.h"
#include "search.h"
#include "search_cli.h"
#include "generator.h"
#include "kd_tree.h"
#include "visualizer.h"

#define TOTAL_BIOMAS 10
#define MAX_BIOMAS_USUARIO 2
#define MAX_ESTRUCTURAS_USUARIO 2
#define MAX_COORDENADAS_USUARIO 2
#define MAX_RANGO_USUARIO 1

/*
    LINK CUBIOMES: https://github.com/Cubitect/cubiomes 

    PARA EJECUTAR Y COMPILAR:
        mingw32-make
        ./seed_analyzer
*/

// Funciones de comunicación con el usuario


void menuPrincipal(){
    puts("===== SeedAnalyzer Menu =====");
    puts("1. Crear nuevos criterios de búsqueda");
    puts("2. Cargar criterios existentes");
    puts("3. Realizar búsqueda con criterios actuales");
    puts("4. Visualizar Resultados");
    puts("5. Salir del programa");
    puts("=============================");
}

void menuOpcion1(){
    puts("===== Crear Nuevos Criterios =====");
    puts("1. Definir biomas deseados");
    puts("2. Definir estructuras deseadas");
    puts("3. Definir coordenadas específicas");
    puts("4. Definir rango de búsqueda");
    puts("5. Resumen de criterios seleccionados");
    puts("6. Volver al menú principal");
}

void menuOpcion2(){
    puts("====== Cargar Criterios Existentes ======");
    puts("Se encontraron los siguientes archivos .json:");
    puts("1. criteria_montaña.json");
    puts("2. criteria_oceano.json");
    puts("3. criteria_village_bioma.json");
}

void menuOpcion3(){
    puts("====== Realizar Búsqueda de la Zona =====");
    puts("1. Iniciar búsqueda con criterios actuales");
    puts("2. Modificar semilla de búsqueda");
    puts("3. Ver resultados de la búsqueda");
    puts("4. Volver al menú principal");
}

void menuOpcion4(){
    puts("====== Visualización de Zonas Candidatas =====");
    puts("1. Visualización 2D con Cubiomes");
    puts("2. Volver al menú principal");
}

// Funciones interactivas del programa

void ejecutarOpcion1(criterioBusqueda *c){
    int opcion;

    do  {
        limpiarPantalla();
        menuOpcion1();
        printf("\nSeleccione una opción: "); opcion = leerOpcion(1, 6);
        switch(opcion)
        {
            case 1:
                almacenarCriterio(c->biomasRequeridos, "Biomas", MAX_BIOMAS_USUARIO);
                break;
            case 2:
                almacenarCriterio(c->estructurasRequeridas, "Estructuras", MAX_ESTRUCTURAS_USUARIO);
                break;
            case 3:
                almacenarCriterio(c->coordenadasIniciales, "Coordenadas", MAX_COORDENADAS_USUARIO);
                break;
            case 4:
                almacenarCriterio(c->radioBusquedaEnChunks, "Rango", MAX_RANGO_USUARIO);
                break;
            case 5:
                resumenCriterios(c, 1);
                break;
            case 6:
                puts("Volviendo al menú principal...");
                break;
            default:
                puts("Opción no válida.");
                presioneEnterParaContinuar();
        }
    }   while (opcion != 6);
}


void ejecutarOpcion2(criterioBusqueda *c) {
    int opcion;

    do {
        limpiarPantalla();
        menuOpcion2();

        puts("1. Guardar criterios actuales en un archivo .json");
        puts("0. ---> Volver al menu principal");
        puts("========================================");
        printf("\nSeleccione una opcion: ");

        opcion = leerOpcion(0, 1);

        switch (opcion) {
            case 1: {
                // Verificar si hay criterios definidos
                if (list_size(c->biomasRequeridos) == 0 &&
                    list_size(c->estructurasRequeridas) == 0 &&
                    list_size(c->coordenadasIniciales) == 0 &&
                    list_size(c->radioBusquedaEnChunks) == 0) {
                    puts("\nPrimero debes definir tus criterios antes de poder guardarlos :)");
                    break;
                }

                // Continuar con el guardado
                char nombreArchivo[128];
                printf("\nIngrese el nombre del archivo para guardar los criterios: ");
                scanf("%127s", nombreArchivo);

                if (guardarCriteriosEnJSON(c, nombreArchivo)) {
                    puts("\nCriterios guardados exitosamente.");
                } else {
                    puts("\nError al guardar los criterios.");
                }
                break;
            }

            case 0:
                puts("Volviendo al menu principal...");
                break;

            default:
                puts("Opcion invalida.");
        }

        presioneEnterParaContinuar();

    } while (opcion != 0);
}

void ejecutarOpcion3(criterioBusqueda *c,
                     KDTree *arbolito,
                     uint64_t *pseed,
                     RegionResult **lastResultsPtr,
                     int *lastCountPtr) {
    int opcion;

    do {
        limpiarPantalla();
        menuOpcion3();
        printf("\nSeleccione una opción: ");
        opcion = leerOpcion(1, 4);

        switch (opcion) {
            case 1:
                // Ejecutar búsqueda y guardar resultados
                if (*lastResultsPtr) {
                    free(*lastResultsPtr);
                    *lastResultsPtr = NULL;
                    *lastCountPtr  = 0;
                }
                realizarBusqueda(c, pseed, lastResultsPtr, lastCountPtr);
                break;

            case 2:
                // Modificar semilla
                modificarSemilla(pseed);
                break;

            case 3:
                mostrarResultados(c, *pseed, *lastResultsPtr, *lastCountPtr, arbolito );
                break;

            case 4:
                puts("Volviendo al menú principal...");
                break;

            default:
                puts("Opción no válida.");
                presioneEnterParaContinuar();
                break;
        }
    } while (opcion != 4);
}

void ejecutarOpcion4(criterioBusqueda *c, KDTree *arbolito, uint64_t *pseed, RegionResult *lastResults, int lastCount){
    
    int opcion;

    do {
        limpiarPantalla();
        menuOpcion4();
        printf("\nSeleccione una opción: "); opcion = leerOpcion(1, 4);
        switch(opcion)
        {
            case 1:
                visualizer2d(c, *pseed, lastResults, lastCount);
                break;
            case 2: 
                puts("Volviendo al menú principal");
                return;
            default:
                puts("Opción no válida.");
                presioneEnterParaContinuar();
        }
    } while (opcion != 3);
}

int main(){
    criterioBusqueda *criterioUsuario = setDefaultCriterio();
    
    int opcion;
    KDTree *arbolito = kd_create(2);

    uint64_t seed_value = 0;
    uint64_t *pseed = &seed_value;

    RegionResult *lastResults = NULL;
    int lastCount = 0;

    do
    {
        limpiarPantalla();
        menuPrincipal();
        printf("Seleccione una opción: "); opcion = leerOpcion(1, 5);
        
        switch(opcion)
        {
            case 1:
                ejecutarOpcion1(criterioUsuario);
                break;
            case 2:
                //ejecutarOpcion2(criterioUsuario);
                break;
            case 3:
                ejecutarOpcion3(
                        criterioUsuario,
                        arbolito,
                        pseed,
                        &lastResults,   // RegionResult**
                        &lastCount      // int*
                    );
                break;
            case 4:
                ejecutarOpcion4(criterioUsuario, arbolito, pseed, lastResults, lastCount);
                break;
            case 5:
                puts("Saliendo del programa...");
                break;
            default:
                puts("Opción no válida, por favor intente de nuevo.");
        }
        presioneEnterParaContinuar();
    } while(opcion != 5);
    
    return 0;
}
