#include "criteria.h"
#include "list.h"
#include "extra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BIOMAS_USUARIO 2
#define MAX_ESTRUCTURAS_USUARIO 2
#define MAX_COORDENADAS_USUARIO 2
#define MAX_RANGO_USUARIO 1

#define TOTAL_BIOMAS 10
#define TOTAL_ESTRUCTURAS 7


/* 
    Lista de los principales biomas de Minecraft con sus nombres en español y en inglés.
*/
ID Biomas[TOTAL_BIOMAS] = {
    {"Pradera", "plains"},
    {"Bosque", "forest"},
    {"Bosque de abedules", "birch_forest"},
    {"Desierto", "desert"},
    {"Taiga", "taiga"},
    {"Selva", "jungle"},
    {"Sabana", "savanna"},
    {"Pantano", "swamp"},
    {"Océano", "ocean"},
    {"Bosque oscuro", "dark_forest"},
};


/* 
    Lista de estructuras disponibles con nombres en español y en inglés 
*/
ID Estructuras[TOTAL_ESTRUCTURAS] = {
    {"Aldea", "village"},
    {"Templo del desierto", "desert_pyramid"},
    {"Templo de la jungla", "jungle_pyramid"},
    {"Poste de saqueadores", "pillager_outpost"},
    {"Monumento oceánico", "ocean_monument"},
    {"Resto de portal", "ruined_portal"},
    {"Mina Abandonada", "Mineshaft"}
};


/*
    Función que inicializa un criterio de búsqueda con valores por defecto.
    Se asigna memoria para el criterio y se establecen valores iniciales.
*/
criterioBusqueda* setDefaultCriterio() {
    criterioBusqueda *c = malloc(sizeof(criterioBusqueda));

    if (c == NULL) {
        fprintf(stderr, "Error al asignar memoria para criterioBusqueda.\n");
        exit(EXIT_FAILURE);
    }
    c->radioBusquedaEnChunks = list_create();
    c->coordenadasIniciales = list_create();
    c->biomasRequeridos = list_create();
    c->estructurasRequeridas = list_create();
    return c;
}

/* 
    Muestra la lista de biomas disponibles por consola 
*/
void mostrarBiomasDisponibles(ID listaBiomas[]) {
    for (int i = 0; i < TOTAL_BIOMAS; i++) {
        printf("%d. %s (%s)\n", i + 1, listaBiomas[i].nombreEsp, listaBiomas[i].nombreCub);
    }
}

/* 
    Muestra la lista de estructuras disponibles por consola 
*/
void mostrarEstructurasDisponibles(ID listaEstructuras[]) {
    for (int i = 0; i < TOTAL_ESTRUCTURAS; i++) {
        printf("%d. %s (%s)\n", i + 1, listaEstructuras[i].nombreEsp, listaEstructuras[i].nombreCub);
    }
}


/*
     Compara dos estructuras ID usando el nombre en inglés (nombreCub).
     Retorna 0 si son iguales, distinto de 0 si son diferentes.
 */
int compararID(void *ID1, void *ID2) {
    return strcmp(((ID *)ID1)->nombreCub, ((ID *)ID2)->nombreCub);
}


/*
     Elimina todas las selecciones dentro de una lista específica
     después de confirmar con el usuario.
 */
void eliminarTodasLasSelecciones(List *listaCriterios) {
    limpiarPantalla();
    puts("========= Eliminar Todas las Selecciones =========");

    if (listaCriterios == NULL || list_size(listaCriterios) == 0) {
        puts("No hay selecciones para eliminar.");
        presioneEnterParaContinuar();
        return;
    }

    printf("¿Estás seguro de que deseas eliminar todas las selecciones? (s/n): ");
    fflush(stdout);

    char buffer[32];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        puts("Entrada inválida.");
        presioneEnterParaContinuar();
        return;
    }

    // Validar que la entrada sea exactamente "s\n" o "S\n"
    if (strcmp(buffer, "s\n") != 0 && strcmp(buffer, "S\n") != 0) {
        puts("Eliminación cancelada.");
        presioneEnterParaContinuar();
        return;
    }

    list_clean(listaCriterios);
    puts("Todas las selecciones han sido eliminadas.");
    presioneEnterParaContinuar();
}


/*
     Permite al usuario ingresar coordenadas o rango manualmente por consola.
     Esta función es llamada para criterios que no tienen una lista fija de opciones (como coordenadas o rango).
 */
void seleccionPorConsola(List* listaCriterio, char* nombreCriterio, int MAX) {
    
    if (strcmp(nombreCriterio, "Coordenadas") == 0){
        
        int x, z;
        puts("Advertencia: Las coordenadas deben estar en el rango de 0 a 500, tanto en X como en Z");

        printf("\nIngrese la coordenada inicial en x: ");
        x = leerOpcion(0, 1000);
        if (x == -1)
        {
            puts("Entrada no válida. Debe ingresar un número entre -1000 y 1000.");
            presioneEnterParaContinuar();
            return;
        }

        printf("\nIngrese la coordenada inicial en z: ");
        z = leerOpcion(-1000, 1000);
        if (z == -1)
        {
            puts("Entrada no válida. Debe ingresar un número entre -1000 y 1000.");
            presioneEnterParaContinuar();
            return;
        }

        int *xptr = malloc(sizeof(int));
        if (xptr == NULL)
        {
            puts("Error al asignar memoria para la coordenada x.");
            presioneEnterParaContinuar();
            return;
        }

        int *zptr = malloc(sizeof(int));
        if (zptr == NULL)
        {
            puts("Error al asignar memoria para la coordenada z.");
            presioneEnterParaContinuar();
            return;
        }

        *xptr = x;
        *zptr = z;

        list_pushBack(listaCriterio, xptr);
        list_pushBack(listaCriterio, zptr);

        printf("Coordenadas (%d, %d) agregadas correctamente.\n", x, z);
        presioneEnterParaContinuar();
        return;
    }

    if (strcmp(nombreCriterio, "Rango") == 0){
        
        int rango;
        puts("Advertencias: El rango se mide en chunks (regiones de 16 x 16 bloques).");
        puts("              El rango debe ser menor a 1028 Chunks.");

        printf("\nIngrese el rango de búsqueda en chunks: ");
        rango = leerOpcion(1, 1028);

        if (rango == -1)
        {
            puts("Entrada no válida. Intente de nuevo.");
            presioneEnterParaContinuar();
            return;
        }

        int *rangoPtr = malloc(sizeof(int));
        if (rangoPtr == NULL)
        {
            puts("Error al asignar memoria para el rango.");
            presioneEnterParaContinuar();
            return;
        }

        *rangoPtr = rango;

        list_pushFront(listaCriterio, rangoPtr);
        printf("Rango de búsqueda de %d chunks agregado correctamente.\n", rango);
        presioneEnterParaContinuar();
        return;
    }
}


/*
     Muestra las opciones disponibles para biomas o estructuras según el criterio especificado.
     Retorna 1 si hay opciones para mostrar, 0 si el criterio requiere entrada manual, y -1 si es inválido.
 */
int mostrarOpcionesSeleccion(char* nombreCriterio)
{
    if (strcmp(nombreCriterio, "Biomas") == 0) { mostrarBiomasDisponibles(Biomas); return 1;}
    else if (strcmp(nombreCriterio, "Estructuras") == 0) { mostrarEstructurasDisponibles(Estructuras); return 1;}
    else if (strcmp(nombreCriterio, "Coordenadas") == 0) { return 0; }
    else if (strcmp(nombreCriterio, "Rango") == 0) { return 0; }

    else{ return -1; }
}

/*
     Permite al usuario agregar una nueva selección a la lista del criterio actual.
     Maneja tanto selección desde una lista como ingreso manual.
 */
void nuevaSeleccion(List* listaCriterio, char* nombreCriterio, int MAX) {
    
    limpiarPantalla();

    if (list_size(listaCriterio) >= MAX)
    {
        puts("===============================================");   
        puts("Error: Se ha alcanzado el máximo de selecciones permitidas.");
        puts("Por favor, elimine una selección antes de agregar una nueva.");
        presioneEnterParaContinuar();
        return;
    }

    do
    {
        puts("===============================================");
        printf("Nueva selección de %s\n", nombreCriterio);
        puts("===============================================");

        int flag;
        flag = mostrarOpcionesSeleccion(nombreCriterio);

        if (flag == -1) {
            puts("Error: Criterio de búsqueda no reconocido.");
            presioneEnterParaContinuar();
            return;
        }

        if (flag == 0) {
            seleccionPorConsola(listaCriterio, nombreCriterio, MAX);
            return;
        }

        if (flag == 1){

            int TOTAL;
            if (strcmp(nombreCriterio, "Biomas") == 0) {TOTAL = TOTAL_BIOMAS;}
            else if (strcmp(nombreCriterio, "Estructuras") == 0) {TOTAL = TOTAL_ESTRUCTURAS;}
            
            printf("Seleccione su opción (1-%d): ", TOTAL);
            int opcion = leerOpcion(1, TOTAL);

            if (opcion == -1)
            {
                printf("\nEntrada no válida. Debe ingresar un número entre 1 y %d.\n", TOTAL);
                presioneEnterParaContinuar();
                return;
            }

            ID *seleccionado;
            seleccionado = malloc(sizeof(ID));
            if (seleccionado == NULL)
            {
                puts("Error al asignar memoria para la selección.");
                presioneEnterParaContinuar();
                return;
            }

            if (strcmp(nombreCriterio, "Biomas") == 0)
            {
                strcpy(seleccionado->nombreEsp, Biomas[opcion - 1].nombreEsp);
                strcpy(seleccionado->nombreCub, Biomas[opcion - 1].nombreCub);
            }
            else if (strcmp(nombreCriterio, "Estructuras") == 0)
            {
                strcpy(seleccionado->nombreEsp, Estructuras[opcion - 1].nombreEsp);
                strcpy(seleccionado->nombreCub, Estructuras[opcion - 1].nombreCub);
            }

            if (list_exist(listaCriterio, seleccionado, compararID))
            {
                puts("Este elemento ya ha sido seleccionado.");
                presioneEnterParaContinuar();
                return;
            }

            list_pushBack(listaCriterio, seleccionado);
            printf("\n'%s' agregada correctamente a la selección de %s.\n", seleccionado->nombreEsp, nombreCriterio);
            presioneEnterParaContinuar();
            return;

        }
    } while (1);
}


/*
     Muestra el submenú para gestionar un criterio específico (agregar, ver, eliminar).
 */
void menuOpcionesCriterio(char* nombreCriterio) {
    printf("1. Nueva selección de %s\n", nombreCriterio);
    printf("2. Mostrar la selección de %s\n", nombreCriterio);
    printf("3. Eliminar una selección\n");
    printf("4. Eliminar todas las selecciones\n");
    printf("5. Volver al menú de selección de criterios\n");
}


/*
    Muestra en pantalla la selección actual de un criterio.
    Adapta la presentación dependiendo si es un ID, coordenada o número.
 */
void mostrarSeleccion(List* listaCriterio, char* nombreCriterio, int flag){
    if (flag == 1) {limpiarPantalla();}

    if (flag == 1){
        printf("========= Selección de %s =========\n", nombreCriterio);
    }
    
    if (listaCriterio == NULL || list_size(listaCriterio) == 0) {
        puts("No hay selecciones almacenadas.");
        if (flag == 1) {presioneEnterParaContinuar();}
        return;
    }

    if (strcmp(nombreCriterio, "Biomas") == 0 || strcmp(nombreCriterio, "Estructuras") == 0){
        ID *seleccion = list_first(listaCriterio);
        int i = 1;
        while (seleccion != NULL) {
            printf("%d. %s (%s)\n", i++, seleccion->nombreEsp, seleccion->nombreCub);
            seleccion = list_next(listaCriterio);
        }
        if (flag == 1) {presioneEnterParaContinuar();}
        return;
    }
    
    if (strcmp(nombreCriterio, "Coordenadas") == 0) {
        int *x = list_index(listaCriterio, 0);
        int *z = list_index(listaCriterio, 1);

        printf("Coordenadas iniciales para la búsqueda:\n");
        printf("Coordenada X: %d\n", *x);
        printf("Coordenada Z: %d\n", *z);
        if (flag == 1) {presioneEnterParaContinuar();}
        return;
    }
    
    if (strcmp(nombreCriterio, "Rango") == 0) {
        int *rango = list_first(listaCriterio);
        printf("Rango de búsqueda: %d chunks\n", *rango);
        if (flag == 1) {presioneEnterParaContinuar();}
        return;
    }

}


/*
     Permite eliminar una selección específica del criterio actual.
     Para coordenadas y rango, limpia toda la lista directamente.
 */
void eliminarSeleccion(List* listaCriterio, char* nombreCriterio) {
    limpiarPantalla();
    printf("========= Eliminar Una Selección de %s =========\n", nombreCriterio);

    if (listaCriterio == NULL || list_size(listaCriterio) == 0) {
        puts("No hay selecciones almacenadas para eliminar.");
        presioneEnterParaContinuar();
        return;
    }

    if (strcmp(nombreCriterio, "Biomas") == 0 || strcmp(nombreCriterio, "Estructuras") == 0)
    {
        mostrarSeleccion(listaCriterio, nombreCriterio, 0);
        printf("\nSeleccione el número de la selección que desea eliminar (0 para cancelar): ");

        int opcion = leerOpcion(0, list_size(listaCriterio));
        if (opcion == 0)
        {
            puts("Eliminación cancelada.");
            presioneEnterParaContinuar();
            return;
        }

        if (opcion == -1)
        {
            printf("\nEntrada no válida. Debe ingresar un número entre 1 y %d.\n", list_size(listaCriterio));
            presioneEnterParaContinuar();
            return;
        }

        ID *seleccion = list_index(listaCriterio, opcion - 1);
        list_popCurrent(listaCriterio);
        printf("Selección '%s' eliminada correctamente.\n", seleccion->nombreEsp);
        presioneEnterParaContinuar();
        return;
    }

    if (strcmp(nombreCriterio, "Coordenadas") == 0 || strcmp(nombreCriterio, "Rango") == 0) {
        list_clean(listaCriterio);
        printf("Selección de %s eliminada correctamente.\n", nombreCriterio);
        presioneEnterParaContinuar();
        return;
    }
}


/*
     Muestra el menú interactivo para definir o modificar las selecciones del usuario
     en relación a un criterio específico (biomas, estructuras, coordenadas o rango).
 */
void almacenarCriterio(List* listaCriterio, char* nombreCriterio, int MAX){
    limpiarPantalla();
    
    if (listaCriterio == NULL || nombreCriterio == NULL) {
        puts("===============================================");
        puts("Error: Criterio de búsqueda o el nombre del criterio son nulos :(");
        presioneEnterParaContinuar();
        return;
    }

    int opcion;
    do
    {
        limpiarPantalla();
        puts("===============================================");
        printf("  Definir criterio de búsqueda: %s\n", nombreCriterio);
        puts("===============================================");

        menuOpcionesCriterio(nombreCriterio);
        printf("Seleccione su opción (1-5): ");
        opcion = leerOpcion(1, 5);

        switch(opcion)
        {
            case 1:
                nuevaSeleccion(listaCriterio, nombreCriterio, MAX);
                break;
            case 2:
                mostrarSeleccion(listaCriterio, nombreCriterio, 1);
                break;
            case 3:
                eliminarSeleccion(listaCriterio, nombreCriterio);
                break;
            case 4:
                eliminarTodasLasSelecciones(listaCriterio);
                break;
            case 5:
                puts("Volviendo al menú principal...");
                presioneEnterParaContinuar();
                return;
            default:
                puts("Opción no válida, por favor intente de nuevo.");
                presioneEnterParaContinuar();
                break;
        }
    } while (1);


}

/*
     Muestra un resumen completo de los criterios actuales definidos por el usuario,
     incluyendo biomas, estructuras, coordenadas y rango.
 */
void resumenCriterios(criterioBusqueda *criterioUsuario, int flag) {
    
    limpiarPantalla();
    puts("========= Resumen de Criterios de Búsqueda =========");
    if (criterioUsuario == NULL)
    {
        puts("No se ha definido ningún criterio de búsqueda.");
        presioneEnterParaContinuar();
        return;
    }

    puts("==== Biomas requeridos ====");
    mostrarSeleccion(criterioUsuario->biomasRequeridos, "Biomas", 0);

    puts("\n==== Estructuras requeridas ====");
    mostrarSeleccion(criterioUsuario->estructurasRequeridas, "Estructuras", 0);

    puts("\n==== Coordenadas iniciales ====");
    if (list_size(criterioUsuario->coordenadasIniciales) == 0){
        puts("Coordenada X = 0");
        puts("Coordenada Z = 0");
    }
    else { mostrarSeleccion(criterioUsuario->coordenadasIniciales, "Coordenadas", 0); }

    puts("\n==== Rango de búsqueda ====");
    if (list_size(criterioUsuario->radioBusquedaEnChunks) == 0){
        puts("Rango de búsqueda: 1028 chunks [RANGO POR DEFECTO]");
    }
    else { mostrarSeleccion(criterioUsuario->radioBusquedaEnChunks, "Rango", 0); }

    if (flag == 1) {puts("\n===============================================");}
    if (flag == 1) {presioneEnterParaContinuar();}
    return;
}