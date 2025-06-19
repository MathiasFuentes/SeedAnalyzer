#include "extra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void presioneEnterParaContinuar() {
    printf("Presione Enter para continuar...");
    fflush(stdout); // Asegura que se imprima antes de esperar
    while (getchar() != '\n'); // Espera hasta que se presione Enter
}


void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int leerOpcion(int min, int max) {
    char buffer[100];
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1; // Lectura fallida
    }

    // Eliminar el salto de línea
    buffer[strcspn(buffer, "\n")] = '\0';

    // Verificar que el buffer no esté vacío y tenga solo dígitos
    size_t len = strlen(buffer);
    if (len == 0) return -1;

    for (size_t i = 0; i < len; i++) {
        if (!isdigit((unsigned char)buffer[i])) {
            return -1; // Contiene caracteres no numéricos
        }
    }

    // Convertir a entero
    int opcion = atoi(buffer);
    if (opcion >= min && opcion <= max) {
        return opcion;
    }

    return -1; // Número fuera de rango
}