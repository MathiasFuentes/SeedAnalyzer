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



// Menú principal con las opciones globales del programa
void menuPrincipal(){
    puts("===== SeedAnalyzer Menu =====");
    puts("1. Crear nuevos criterios de busqueda");
    puts("2. Cargar criterios existentes");
    puts("3. Realizar busqueda con criterios actuales");
    puts("4. Visualizar Resultados");
    puts("5. Salir del programa");
    puts("=============================");
}

// Menú para definir nuevos criterios de búsqueda
void menuOpcion1(){
    puts("===== Crear Nuevos Criterios =====");
    puts("1. Definir biomas deseados");
    puts("2. Definir estructuras deseadas");
    puts("3. Definir coordenadas especificas");
    puts("4. Definir rango de busqueda");
    puts("5. Resumen de criterios seleccionados");
    puts("6. Volver al menú principal");
}

// Menú para guardar o cargar criterios desde archivo
void menuOpcion2(){
    puts("====== Cargar Criterios Recibidos ======");
    puts("Estos son algunos ejemplos de archivos .json:");
    puts(" --> criteria_montana.json");
    puts(" --> criteria_oceano.json");
    puts(" --> criteria_village_bioma.json\n");
    puts("Seleccione una opcion (0,1)");
}

// Menú para ejecutar la búsqueda con criterios definidos
void menuOpcion3(){
    puts("====== Realizar Búsqueda de la Zona =====");
    puts("1. Iniciar busqueda con criterios actuales");
    puts("2. Modificar semilla de busqueda");
    puts("3. Ver resultados de la busqueda");
    puts("4. Volver al menu principal");
}

// Menú para visualización de resultados (ej. Cubiomes)
void menuOpcion4(){
    puts("====== Visualización de Zonas Candidatas =====");
    puts("1. Visualización 2D con Cubiomes");
    puts("2. Volver al menú principal");
}


// Ejecuta las acciones relacionadas con la definición de criterios de búsqueda
void ejecutarOpcion1(criterioBusqueda *c){
    int opcion;

    do  {
        limpiarPantalla();
        menuOpcion1();
        printf("\nSeleccione una opcion: "); opcion = leerOpcion(1, 6);
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
                puts("Volviendo al menu principal...");
                break;
            default:
                puts("Opción no valida.");
                presioneEnterParaContinuar();
        }
    }   while (opcion != 6);
}


// Permite guardar los criterios actuales a un archivo .json
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
                    presioneEnterParaContinuar();
                    return;
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
                presioneEnterParaContinuar();
                return;
            }

            case 0:
                puts("Volviendo al menu principal...");
                presioneEnterParaContinuar();
                return;

            default:
                puts("Opcion invalida.");
                presioneEnterParaContinuar();
                break;
        }
    } while (1);
}

// Realiza la búsqueda con los criterios definidos, permite modificar la semilla o visualizar resultados
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
                puts("Volviendo al menu principal...");
                break;

            default:
                puts("Opcion no valida.");
                presioneEnterParaContinuar();
                break;
        }
    } while (opcion != 4);
}


// Llama a la visualización 2D de los resultados con Cubiomes
void ejecutarOpcion4(criterioBusqueda *c, KDTree *arbolito, uint64_t *pseed, RegionResult *lastResults, int lastCount){
    
    int opcion;

    do {
        limpiarPantalla();
        menuOpcion4();
        printf("\nSeleccione una opcion: "); opcion = leerOpcion(1, 4);
        switch(opcion)
        {
            case 1:
                visualizer2d(c, *pseed, lastResults, lastCount);
                break;
            case 2: 
                puts("Volviendo al menú principal");
                return;
            default:
                puts("Opcion no valida.");
                presioneEnterParaContinuar();
        }
    } while (opcion != 3);
}


/*
    Función principal: controla el ciclo de ejecución del programa.
    - Inicializa estructuras de criterios y árbol KD.
    - Permite al usuario navegar por el menú principal.
    - Gestiona las llamadas a las distintas funcionalidades.
*/
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
        printf("Seleccione una opcion: "); opcion = leerOpcion(1, 5);
        
        switch(opcion)
        {
            case 1:
                ejecutarOpcion1(criterioUsuario);
                break;
            case 2:
                ejecutarOpcion2(criterioUsuario);
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
                puts("Opcion no valida, por favor intente de nuevo.");
        }
        presioneEnterParaContinuar();
    } while(opcion != 5);
    
    return 0;
}
