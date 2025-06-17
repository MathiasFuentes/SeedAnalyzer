#include <stdio.h>
#include <stdlib.h>
#include "extra.h"
#include "criteria.h"

/*
    LINK CUBIOMES: https://github.com/Cubitect/cubiomes 

    PARA EJECUTAR Y COMPILAR:
        mingw32-make
        ./seed_analyzer
*/

// Primeros interactuables del programa.

int leerOpcion(int min, int max) {
    int opcion;
    while (1) {
        printf("Ingrese una opción (%d-%d): ", min, max);
        if (scanf("%d", &opcion) != 1 || opcion < min || opcion > max) {
            printf("Opción inválida. Intente de nuevo.\n");
            while(getchar() != '\n'); // Limpiar el buffer
        } else {
            break;
        }
    }
    return opcion;
}

void menuPrincipal(){
    puts("===== SeedAnalyzer Menu =====");
    puts("1. Crear nuevos criterios de búsqueda");
    puts("2. Cargar criterios existentes");
    puts("3. Ejecutar análisis con criterios actuales");
    puts("4. Visualizar Resultados");
    puts("5. Salir");
    puts("=============================");
}

void menuOpcion1(){
    puts("===== Crear Nuevos Criterios =====");
    puts("1. Definir biomas deseados");
    puts("2. Definir estructuras deseadas");
    puts("3. Definir coordenadas específicas");
    puts("4. Definir rango de búsqueda");
    puts("5. Volver al menú principal");
    puts("===================================");
}

void menuOpcion2(){
    puts("OPCION NO IMPLEMENTADA");
}

void menuOpcion3(){
    puts("OPCION NO IMPLEMENTADA");
}

void menuOpcion4(){
    puts("OPCION NO IMPLEMENTADA");
}

// 

void ejecutarOpcion1(){
    limpiarPantalla();
    menuOpcion1();

    // Se inicializa un criterio de búsqueda con valores por defecto.
    criterioBusqueda *criterioUsuario = setDefaultCriterio();

    int opcion = leerOpcion(1,5);
    do {
        switch(opcion) {
            case 1:
                // Aquí iría la lógica para definir biomas
                break;
            case 2:
                // Aquí iría la lógica para definir estructuras
                break;
            case 3:
                // Aquí iría la lógica para definir coordenadas
                break;
            case 4:
                // Aquí iría la lógica para definir el rango
                break;
            case 5:
                puts("Volviendo al menú principal...");
                break;
            default:
                puts("Opción no válida.");
        }
     }  while (opcion != 5);
}

int main(){
    int opcion;
    do {
        menuPrincipal();
        printf("Seleccione una opción: ");
        opcion = leerOpcion(1, 5);
        limpiarPantalla();
        switch(opcion) {
            case 1:
                ejecutarOpcion1();
                break;
            case 2:
                menuOpcion2();
                break;
            case 3:
                menuOpcion3();
                break;
            case 4:
                menuOpcion4();
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