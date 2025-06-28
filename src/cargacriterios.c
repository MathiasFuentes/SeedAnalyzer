#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "criteria.h"
#include "cargacriterios.h"
#include "extra.h"
#include "cJSON.h"
#include "list.h"

#define CRITERIA_DIR "./criteria/"
#define MAX_ARCHIVOS 100

// Divide una cadena separada por comas y agrega los valores a la lista correspondiente.
// Se utiliza para biomas y estructuras.
// Si el tipo es "bioma" o "estructura", se reserva memoria para un ID y se almacena el nombre.
void splitYAgregarALista(List *lista, const char *str, const char *tipo) {
    char copia[512];
    strcpy(copia, str);
    char *token = strtok(copia, ",");
    while (token != NULL) {
        if (strcmp(tipo, "bioma") == 0 || strcmp(tipo, "estructura") == 0) {
            ID *nuevo = malloc(sizeof(ID));
            if (nuevo) {
                strcpy(nuevo->nombreCub, token);
                strcpy(nuevo->nombreEsp, token); // si quieres solo en inglés
                list_pushBack(lista, nuevo);
            }
        }
        token = strtok(NULL, ",");
    }
}

// Divide una cadena con coordenadas separadas por coma (ej: "100,-300").
// Cada valor se convierte a entero y se agrega a la lista como puntero a entero.
void splitYAgregarCoordenadas(List *lista, const char *str) {
    char copia[512];
    strcpy(copia, str);
    char *token = strtok(copia, ",");
    while (token != NULL) {
        int *valor = malloc(sizeof(int));
        if (valor) {
            *valor = atoi(token);
            list_pushBack(lista, valor);
        }
        token = strtok(NULL, ",");
    }
}

// Lee criterios de búsqueda desde un archivo de texto (.txt o .cfg).
int leerCriterioDesdeArchivo(criterioBusqueda *c, const char *ruta) {
    FILE *fp = fopen(ruta, "r");
    if (fp == NULL) {
        perror("Error abriendo archivo");
        return 0;
    }

    // Limpiar listas actuales
    list_clean(c->biomasRequeridos);
    list_clean(c->estructurasRequeridas);
    list_clean(c->coordenadasIniciales);
    list_clean(c->radioBusquedaEnChunks);

    char linea[512];
    while (fgets(linea, sizeof(linea), fp)) {
        linea[strcspn(linea, "\n")] = 0;

        if (strncmp(linea, "biomas=", 7) == 0) {
            splitYAgregarALista(c->biomasRequeridos, linea + 7, "bioma");
        } else if (strncmp(linea, "estructuras=", 12) == 0) {
            splitYAgregarALista(c->estructurasRequeridas, linea + 12, "estructura");
        } else if (strncmp(linea, "coordenadas=", 12) == 0) {
            splitYAgregarCoordenadas(c->coordenadasIniciales, linea + 12);
        } else if (strncmp(linea, "rango=", 6) == 0) {
            int *rango = malloc(sizeof(int));
            if (rango) {
                *rango = atoi(linea + 6);
                list_pushBack(c->radioBusquedaEnChunks, rango);
            }
        }
    }

    fclose(fp);
    return 1;
}

// Guarda los criterios actuales en un archivo JSON.
// Usa la biblioteca cJSON para generar un objeto estructurado.
int guardarCriteriosEnJSON(criterioBusqueda *c, const char *nombreArchivo) {
    if (!c || !nombreArchivo) return 0;

    cJSON *jsonRoot = cJSON_CreateObject();

    // Biomas
    cJSON *biomas = cJSON_CreateArray();
    for (void *ptr = list_first(c->biomasRequeridos); ptr != NULL; ptr = list_next(c->biomasRequeridos)) {
        cJSON_AddItemToArray(biomas, cJSON_CreateString((char *)ptr));
    }
    cJSON_AddItemToObject(jsonRoot, "biomas", biomas);

    // Estructuras
    cJSON *estructuras = cJSON_CreateArray();
    for (void *ptr = list_first(c->estructurasRequeridas); ptr != NULL; ptr = list_next(c->estructurasRequeridas)) {
        cJSON_AddItemToArray(estructuras, cJSON_CreateString((char *)ptr));
    }
    cJSON_AddItemToObject(jsonRoot, "estructuras", estructuras);

    // Coordenadas
    cJSON *coordenadas = cJSON_CreateArray();
    for (void *ptr = list_first(c->coordenadasIniciales); ptr != NULL; ptr = list_next(c->coordenadasIniciales)) {
        cJSON_AddItemToArray(coordenadas, cJSON_CreateNumber(*(int *)ptr));
    }
    cJSON_AddItemToObject(jsonRoot, "coordenadas", coordenadas);

    // Radio
    int *radioPtr = list_first(c->radioBusquedaEnChunks);
    cJSON_AddNumberToObject(jsonRoot, "radio", radioPtr ? *radioPtr : 0);

    // Generar JSON formateado
    char *jsonString = cJSON_Print(jsonRoot);

    // Escribir a archivo
    FILE *file = fopen(nombreArchivo, "w");
    if (!file) {
        cJSON_Delete(jsonRoot);
        free(jsonString);
        return 0;
    }

    fputs(jsonString, file);
    fclose(file);

    // Liberar memoria
    cJSON_Delete(jsonRoot);
    free(jsonString);

    return 1;  // Éxito
}
