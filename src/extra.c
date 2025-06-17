#include "extra.h"
#include <stdio.h>
#include <stdlib.h>

void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void presioneEnterParaContinuar() {
    printf("Presione Enter para continuar...");
    getchar();
}

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}