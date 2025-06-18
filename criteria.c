#include "criteria.h"
#include "list.h"
#include "extra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    Función que muestra los biomas disponibles para que el usuario pueda seleccionar.
    Se recorre la lista de biomas y se imprime su nombre en español y en inglés.
*/

void mostrarBiomasDisponibles(BiomaNombre listaBiomas[]) {
    for (int i = 0; i < TOTAL_BIOMAS; i++) {
        printf("%d. %s (%s)\n", i + 1, listaBiomas[i].nombreEsp, listaBiomas[i].nombreCub);
    }
}


/*
    Función auxiliar que compara dos biomas por su nombre técnico.
    Se utiliza para ordenar o buscar biomas en la lista.
*/

int compararBiomas(void *bioma1, void *bioma2) {
    return strcmp(((BiomaNombre *)bioma1)->nombreCub, ((BiomaNombre *)bioma2)->nombreCub);
}


/*
    Función que muestra los biomas seleccionados por el usuario.
    Si no hay biomas seleccionados, se informa al usuario.
*/

void mostrarBiomasSeleccionados(criterioBusqueda *c) {
    puts("========= Biomas Seleccionados =========");
    
    // Verifica si el criterio de búsqueda o la lista de biomas es nula
    if (c == NULL || c->biomasRequeridos == NULL || list_size(c->biomasRequeridos) == 0) 
    {
        puts("No hay biomas seleccionados.");
        presioneEnterParaContinuar();
        return;
    }

    // Recorre la lista de biomas seleccionados y los muestra
    BiomaNombre *bioma = list_first(c->biomasRequeridos);
    int i = 1;
    while (bioma != NULL)
    {
        printf("%d. %s (%s)\n", i++, bioma->nombreEsp, bioma->nombreCub);
        bioma = list_next(c->biomasRequeridos);
    }
    presioneEnterParaContinuar();
}

/*
    Función que agrega un bioma a la lista de biomas requeridos del criterio de búsqueda.
    Muestra una lista de biomas disponibles y permite al usuario seleccionar uno.
    Si ya se han agregado 3 biomas, no permite agregar más.
    Si el bioma ya existe en la lista, informa al usuario y no lo agrega.
*/

void agregarBioma(criterioBusqueda *c, BiomaNombre listaBiomas[]) {
    limpiarPantalla();
    puts("========= Agregar Bioma =========");

    // Error de memoria
    if (c == NULL || c->biomasRequeridos == NULL)
    {
        puts("Error: El criterio de búsqueda o la lista de biomas es nula");
        presioneEnterParaContinuar();
        return;
    }

    // Verifica si ya se han agregado 3 biomas
    if (list_size(c->biomasRequeridos) >= 3)
    {
        puts("Ya se han agregado 3 biomas. No se pueden agregar más.");
        presioneEnterParaContinuar();
        return;
    }

    // Muestra los biomas disponibles
    puts("Seleccione uno de los siguientes biomas para agregar:");
    mostrarBiomasDisponibles(listaBiomas);
    puts("");

    // Solicita al usuario que ingrese el número del bioma
    int opcion;
    printf("Ingrese el número del bioma (1-%d): ", TOTAL_BIOMAS);

    // Verifica si la entrada es válida
    if (scanf("%d", &opcion) != 1)
    {
        puts("Entrada no válida.");
        while (getchar() != '\n'); // Limpiar buffer
        presioneEnterParaContinuar();
        return;
    }

    // Guarda el bioma seleccionado
    BiomaNombre nuevoBioma;
    strcpy(nuevoBioma.nombreEsp, listaBiomas[opcion - 1].nombreEsp);
    strcpy(nuevoBioma.nombreCub, listaBiomas[opcion - 1].nombreCub);

    // Verifica si el bioma ya existe en la lista de biomas requeridos
    if (list_exist(c->biomasRequeridos, &nuevoBioma, compararBiomas))
    {
        puts("Este bioma ya fue agregado, selecciona otro.");
        presioneEnterParaContinuar();
        return;
    }

    // Asigna memoria para el nuevo bioma y lo agrega a la lista
    BiomaNombre *biomaCopiado = malloc(sizeof(BiomaNombre));
    if (!biomaCopiado)
    {
        puts("Error al asignar memoria para el bioma.");
        presioneEnterParaContinuar();
        return;
    }


    *biomaCopiado = nuevoBioma;
    list_pushBack(c->biomasRequeridos, biomaCopiado);
    printf("Bioma '%s' agregado correctamente.\n", nuevoBioma.nombreEsp);
    presioneEnterParaContinuar();
}

void eliminarBioma(criterioBusqueda *c){
    limpiarPantalla();
    puts("========= Eliminar Bioma =========");
    if (c == NULL || c->biomasRequeridos == NULL) {
        puts("Error: El criterio de búsqueda o la lista de biomas es nula");
        presioneEnterParaContinuar();
        return;
    }

    if (c->biomasRequeridos->size == 0) {
        puts("No hay biomas seleccionados para eliminar.");
        presioneEnterParaContinuar();
        return;
    }

    puts("Biomas seleccionados:");
    BiomaNombre *bioma = list_first(c->biomasRequeridos);
    int i = 1;
    while (bioma != NULL) {
        printf("%d. %s (%s)\n", i++, bioma->nombreEsp, bioma->nombreCub);
        bioma = list_next(c->biomasRequeridos);
    }

    puts("\nSeleccione 0 para cancelar la eliminación.");
    printf("Seleccione el número del bioma que desea eliminar (1-%d): ", c->biomasRequeridos->size);
    int opcion; scanf("%d", &opcion);

    if (opcion == 0) {
        puts("Eliminación cancelada.");
        presioneEnterParaContinuar();
        return;
    }
    if (opcion < 1 || opcion > c->biomasRequeridos->size) {
        puts("Opción no válida.");
        presioneEnterParaContinuar();
        return;
    }

    BiomaNombre eliminar = *(BiomaNombre*)list_index(c->biomasRequeridos, opcion - 1);
    if (list_exist(c->biomasRequeridos, &eliminar, compararBiomas)) {
        list_popCurrent(c->biomasRequeridos);
        printf("Bioma %s eliminado correctamente.\n", eliminar.nombreEsp);
        presioneEnterParaContinuar();
        return;
    } else {
        puts("El bioma seleccionado no existe en la lista.");
        presioneEnterParaContinuar();
        return;
    }

}

void eliminarTodosLosBiomas(criterioBusqueda *c) {
    puts("========= Eliminar Todos los Biomas =========");
    printf("¿Estás seguro de que deseas eliminar todos los biomas seleccionados? (s/n): ");
    char confirmacion;
    scanf(" %c", &confirmacion);
    if (confirmacion != 's' && confirmacion != 'S') {
        puts("Eliminación cancelada.");
        presioneEnterParaContinuar();
        return;
    }

    if (c == NULL || c->biomasRequeridos == NULL) {
        puts("Error: El criterio de búsqueda o la lista de biomas es nula");
        presioneEnterParaContinuar();
        return;
    }
    list_clean(c->biomasRequeridos);
    puts("Todos los biomas han sido eliminados.");
    presioneEnterParaContinuar();
}

void almacenarBiomas(criterioBusqueda *c) {
    limpiarPantalla();

    if (c == NULL){
        puts("========= Selección de Biomas a Buscar =========");
        puts("Error: El criterio de búsqueda es nulo");
        presioneEnterParaContinuar();
        return;
    }

    while (list_size(c->biomasRequeridos) >= MAX_BIOMAS_USUARIO) {
        int opcion;
        puts("Advertencia: Límite de Biomas Seleccionados");
        puts("Opciones:");
        puts("1. Eliminar un bioma seleccionado");
        puts("2. Eliminar todos los biomas seleccionados");
        puts("3. Volver al menú principal");

        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                eliminarBioma(c);
                break;
            case 2:
                limpiarPantalla();
                eliminarTodosLosBiomas(c);
                break;
            case 3:
                return;  // Aquí sí se sale correctamente
            default:
                puts("Opción no válida.");
                break;
        }
        presioneEnterParaContinuar();
    }


    int opcion; 
    do
    {
        limpiarPantalla();
        puts("========= Selección de Biomas a Buscar =========");
        puts("1. Agregar Bioma");
        puts("2. Mostrar Biomas Seleccionados");
        puts("3. Eliminar Bioma");
        puts("4. Eliminar todos los biomas");
        puts("5. Volver al menú principal");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
            case 1:
                limpiarPantalla();
                agregarBioma(c, listaBiomas);
                break;
            case 2:
                limpiarPantalla();
                mostrarBiomasSeleccionados(c);
                break;
            case 3:
                limpiarPantalla();
                eliminarBioma(c);
                break;
            case 4:
                limpiarPantalla();
                eliminarTodosLosBiomas(c);
                break;
            case 5:
                puts("Volviendo al menú principal...");
                break;
            default:
                puts("Opción no válida. Por favor, intente de nuevo.");
                break;
        }
    } while(opcion != 5);
    presioneEnterParaContinuar();
    return;
}

void almacenarCriterios(criterioBusqueda *c) {
    // Implementación pendiente.
    return;
}
