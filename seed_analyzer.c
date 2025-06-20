#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "extra.h"
#include "criteria.h"
#include "cargacriterios.h"

#define TOTAL_BIOMAS 14
#define MAX_BIOMAS_USUARIO 3
#define MAX_ESTRUCTURAS_USUARIO 3
#define MAX_COORDENADAS_USUARIO 2
#define MAX_RANGO_USUARIO 1
/*
    LINK CUBIOMES: https://github.com/Cubitect/cubiomes 

    PARA EJECUTAR Y COMPILAR:
        mingw32-make
        ./seed_analyzer
*/

// Funciones de comunicación con el usuario


void menuPrincipal()
{
    puts("===== SeedAnalyzer Menu =====");
    puts("1. Crear nuevos criterios de búsqueda");
    puts("2. Cargar criterios existentes");
    puts("3. Ejecutar análisis con criterios actuales");
    puts("4. Visualizar Resultados");
    puts("5. Salir del programa");
    puts("=============================");
}

void menuOpcion1()
{
    puts("===== Crear Nuevos Criterios =====");
    puts("1. Definir biomas deseados");
    puts("2. Definir estructuras deseadas");
    puts("3. Definir coordenadas específicas");
    puts("4. Definir rango de búsqueda");
    puts("5. Resumen de criterios seleccionados");
    puts("6. Volver al menú principal");
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
                resumenCriterios(c);
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


int main(){

    criterioBusqueda *criterioUsuario = setDefaultCriterio();
    int opcion;

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
                //menuCargarCriterios(criterioUsuario);
                puts("Cargando criterios existentes...");
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