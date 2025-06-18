#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "criteria.h"
#include "cargacriterios.h"
#include "extra.h"

#define CRITERIA_DIR "./criteria/"
#define MAX_ARCHIVOS 100

void splitYAgregarBiomas(criterioBusqueda *c, const char *str) {
    char copia[512];
    strcpy(copia, str);
    char *token = strtok(copia, ",");
    c->numBiomas = 0;
    while (token != NULL && c->numBiomas < MAX_BIOMAS) {
        strcpy(c->biomas[c->numBiomas], token);
        c->numBiomas++;
        token = strtok(NULL, ",");
    }
}


void splitYAgregarEstructuras(criterioBusqueda *c, const char *str) {
    char copia[512];
    strcpy(copia, str);
    char *token = strtok(copia, ",");
    c->numEstructuras = 0;
    while (token != NULL && c->numEstructuras < MAX_ESTRUCTURAS) {
        strcpy(c->estructuras[c->numEstructuras], token);
        c->numEstructuras++;
        token = strtok(NULL, ",");
    }
}

int leerCriterioDesdeArchivo(criterioBusqueda *c, const char *ruta) {
    FILE *fp = fopen(ruta, "r");
    if (fp == NULL) {
        perror("Error abriendo archivo");
        return 0;
    }

    char linea[512];
    while (fgets(linea, sizeof(linea), fp)) {
        linea[strcspn(linea, "\n")] = 0;

        if (strncmp(linea, "biomas=", 7) == 0) {
            splitYAgregarBiomas(c, linea + 7);
        } else if (strncmp(linea, "estructuras=", 12) == 0) {
            splitYAgregarEstructuras(c, linea + 12);
        } else if (strncmp(linea, "rango=", 6) == 0) {
            c->rango = atoi(linea + 6);
        }
    }

    fclose(fp);
    return 1;
}

void menuCargarCriterios(criterioBusqueda *c) {
    DIR *dir;
    struct dirent *ent;
    char archivos[MAX_ARCHIVOS][256];
    int total = 0;

    dir = opendir(CRITERIA_DIR);
    if (dir == NULL) {
        perror("No se pudo abrir la carpeta de criterios");
        return;
    }

    printf("===== Archivos de criterios disponibles =====\n");
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            printf("%d) %s\n", total + 1, ent->d_name);
            strcpy(archivos[total], ent->d_name);
            total++;
        }
    }
    closedir(dir);

    if (total == 0) {
        printf("No hay archivos de criterios guardados.\n");
        return;
    }

    printf("Seleccione un archivo (1-%d): ", total);
    int opcion = leerOpcion(1, total);

    char ruta[512];
    strcpy(ruta, CRITERIA_DIR);
    strcat(ruta, archivos[opcion - 1]);

    printf("Cargando archivo: %s\n", ruta);

    if (leerCriterioDesdeArchivo(c, ruta)) {
        printf("Criterios cargados correctamente.\n");
    } else {
        printf("Error al cargar el archivo.\n");
    }
}
