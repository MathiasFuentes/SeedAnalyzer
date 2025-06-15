#include <stdio.h>
#include "generator.h"

// Arreglo manual de nombres de biomas (simplificado, puedes expandir)
const char* biome_names[] = {
    "Ocean", "Plains", "Desert", "Mountains", "Forest", "Taiga", "Swamp", "River", "Nether", "The End",
    "Frozen Ocean", "Frozen River", "Snowy Tundra", "Snowy Mountains", "Mushroom Fields", "Mushroom Field Shore",
    "Beach", "Desert Hills", "Wooded Hills", "Taiga Hills", "Mountain Edge", "Jungle", "Jungle Hills",
    "Jungle Edge", "Deep Ocean", "Stone Shore", "Snowy Beach", "Birch Forest", "Birch Forest Hills",
    "Dark Forest", "Snowy Taiga", "Snowy Taiga Hills", "Giant Tree Taiga", "Giant Tree Taiga Hills", "Wooded Mountains"
    // Puedes agregar más si lo necesitas
};

int main() {
    Generator g;
    long long seed = 123456789;
    int biomeId;

    // Inicializa el generador con versión y flags = 0 (sin extras)
    setupGenerator(&g, MC_1_16_1, 0);

    // Aplica la seed
    applySeed(&g, DIM_OVERWORLD, seed);

    // scale = 4 para biomas (más detallado), y = 0 por ser superficie
    biomeId = getBiomeAt(&g, 4, 173, 118, -114);

    // Mostrar el nombre del bioma si está en rango
    if (biomeId >= 0 && biomeId < sizeof(biome_names) / sizeof(biome_names[0])) {
        printf("Bioma en (0,0): %s (ID %d)\n", biome_names[biomeId], biomeId);
    } else {
        printf("Bioma en (0,0): ID desconocido (%d)\n", biomeId);
    }

    return 0;
}
