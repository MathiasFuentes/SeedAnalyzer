#include "extra.h"

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

