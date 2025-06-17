#include "criteria.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_BIOMAS_USUARIO 3
#define TOTAL_BIOMAS 14

/* 
    Lista de los principales biomas de Minecraft con sus nombres en español y en inglés.
*/

BiomaNombre listaBiomas[TOTAL_BIOMAS] = {
    {"Campos de hongos", "mushroom_fields"},
    {"Pradera", "plains"},
    {"Bosque", "forest"},
    {"Bosque de abedules", "birch_forest"},
    {"Desierto", "desert"},
    {"Taiga", "taiga"},
    {"Selva", "jungle"},
    {"Sabana", "savanna"},
    {"Pantano", "swamp"},
    {"Océano", "ocean"},
    {"Océano cálido", "warm_ocean"},
    {"Océano congelado", "frozen_ocean"},
    {"Bosque oscuro", "dark_forest"},
    {"Colinas de piedra", "stone_shore"}
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
    c->numeroAldeas = 0;
    c->radioBusquedaEnChunks = 128; // Valor por defecto, puede ser ajustado
    c->coordenadasIniciales[0] = 0.0; // Coordenadas (0,0) por defecto
    c->coordenadasIniciales[1] = 0.0;
    c->biomasRequeridos = list_create();
    c->estructurasRequeridas = list_create();
    return c;
}


/*
    Función auxiliar que compara dos biomas por su nombre técnico.
    Se utiliza para ordenar o buscar biomas en la lista.
*/

int compararBiomas(void *bioma1, void *bioma2) {
    return strcmp((char *)bioma1, (char *)bioma2);
}


/*
    Función que almacena los biomas requeridos por el usuario.
    Permite seleccionar hasta un máximo de 3 biomas de la lista predefinida.
    Se asigna memoria para los biomas seleccionados y se guardan sus nombres técnicos.
*/

/*
void almacenarBioma(criterioBusqueda *c) {
    
    // Verifica si el usuario ya ha alcanzado el máximo de biomas permitidos.
    int numeroBiomasRequeridos = list_size(c->biomasRequeridos);
    if (numeroBiomasRequeridos >= MAX_BIOMAS_USUARIO) {
        printf("Ya has seleccionado el máximo de biomas permitidos (%d).\n", MAX_BIOMAS_USUARIO);
        puts("Si desea realizar cambios, por favor seleccione una de las siguientes opciones:");
        puts("1.    Eliminar bioma");
        puts("2.    Eliminar todos los biomas");
        puts("3.    Volver al menú principal");

        int opcion;
        printf("Selecciona una opción: ");
        scanf("%d", &opcion);

        if (opcion == 1) {
            list_
            printf("Funcionalidad de eliminación de bioma no implementada.\n");
        } else if (opcion == 2) {
            // Elimina todos los biomas
            list_clean(c->biomasRequeridos);
            numeroBiomasRequeridos = 0;
            printf("Todos los biomas han sido eliminados.\n");
        } else if (opcion == 3) {
            printf("Volviendo al menú principal...\n");
            return;
        } else {
            printf("Opción inválida. Volviendo al menú principal...\n");
            return;
        }
    }
    
    // Imprime la lista de biomas disponibles.
    printf("\nSelecciona hasta %d biomas requeridos:\n", MAX_BIOMAS_USUARIO);
    for (int i = 0; i < TOTAL_BIOMAS; i++) {
        printf("  %2d. %s\n", i + 1, listaBiomas[i].nombreEsp);
    }

    while (numeroBiomasRequeridos < MAX_BIOMAS_USUARIO) {
        int opcion;
        printf("Selecciona un número (0 para terminar): ");
        scanf("%d", &opcion);

        if (opcion == 0) break;

        if (opcion < 1 || opcion > TOTAL_BIOMAS) {
            printf("Opción inválida. Intenta nuevamente.\n");
            continue;
        }

        if (list_exist(c->biomasRequeridos, listaBiomas[opcion - 1].nombreCub, compararBiomas)) {
            printf("El bioma '%s' ya ha sido seleccionado.\n", listaBiomas[opcion - 1].nombreEsp);
            continue;
        }

        // Copiar y agregar el nombre técnico a la lista
        char *nombreTecnico = listaBiomas[opcion - 1].nombreCub;
        char *bioma = malloc(strlen(nombreTecnico) + 1);
        if (bioma == NULL) {
            fprintf(stderr, "Error al asignar memoria para el bioma.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(bioma, nombreTecnico);
        list_add(c->biomasRequeridos, bioma);

        numeroBiomasRequeridos++;
        printf("\nBioma '%s' agregado.\n", listaBiomas[opcion - 1].nombreEsp);
    }
    c->biomasRequeridos->size = numeroBiomasRequeridos;
    printf("Selección completada: %d biomas guardados.\n", numeroBiomasRequeridos);
}


void almacenarCriterios(criterioBusqueda *c) {
    // Implementación pendiente.
    return;
}
*/