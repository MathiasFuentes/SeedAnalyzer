#include <stdio.h>
#include <stdlib.h>
#include "generator.h"

// Biomas más comunes (IDs válidos para MC 1.16.1)
const char* biome_names[] = {
    "Ocean", "Plains", "Desert", "Mountains", "Forest", "Taiga", "Swamp",
    "River", "Nether", "The End", "Frozen Ocean", "Frozen River", "Snowy Tundra",
    "Snowy Mountains", "Mushroom Fields", "Mushroom Field Shore", "Beach",
    "Desert Hills", "Wooded Hills", "Taiga Hills", "Mountain Edge", "Jungle",
    "Jungle Hills", "Jungle Edge", "Deep Ocean", "Stone Shore", "Snowy Beach",
    "Birch Forest", "Birch Forest Hills", "Dark Forest", "Snowy Taiga",
    "Snowy Taiga Hills", "Giant Tree Taiga", "Giant Tree Taiga Hills",
    "Wooded Mountains", "Savanna", "Savanna Plateau", "Badlands", "Wooded Badlands Plateau"
};

#define BIOME_NAME(id) \
    ((id) >= 0 && (id) < sizeof(biome_names)/sizeof(char*)) ? biome_names[(id)] : "Desconocido"

int main() {
    long long seed;
    int x, z;
    /*
    printf("Ingrese seed: ");
    if (scanf("%lld", &seed) != 1) {
        fprintf(stderr, "Error leyendo la seed.\n");
        return 1;
    }

    printf("Ingrese coordenada X: ");
    if (scanf("%d", &x) != 1) {
        fprintf(stderr, "Error leyendo X.\n");
        return 1;
    }

    printf("Ingrese coordenada Z: ");
    if (scanf("%d", &z) != 1) {
        fprintf(stderr, "Error leyendo Z.\n");
        return 1;
    }
    */
    // Valores de ejemplo
    seed = 123456789; // Reemplazar con la seed deseada
    x = 1000; // Reemplazar con la coordenada X deseada
    z = 1000; // Reemplazar con la coordenada Z deseada
    Generator g;
    setupGenerator(&g, MC_1_16_1, 0);
    applySeed(&g, DIM_OVERWORLD, seed);

    int scale = 4; // Resolución de biomas
    int biomeId = getBiomeAt(&g, scale, x, 0, z);

    printf("Bioma en (%d, %d): %s (ID %d)\n", x, z, BIOME_NAME(biomeId), biomeId);

    return 0;
}
