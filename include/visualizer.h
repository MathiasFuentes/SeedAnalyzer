// visualizer.h
#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <stdint.h>

// Inicializa el visualizador con el path donde se guardará la imagen,
// la esquina superior izquierda en chunks (originX, originZ) y el pixscale.
void viz_init(const char *outImagePath,
              int originX, int originZ,
              unsigned pixscale);

// Genera la imagen de biomas 64×64 usando biomesToImage.
// 'cache' es el array de IDs devuelto por genBiomes.
// Llama internamente a parseBiomeColors + biomesToImage + stbi_write_png.
void viz_drawBiomes(const int *cache);

// Marca un punto de interés (chunkX, chunkZ) sobre la imagen.
void viz_addInterest(int chunkX, int chunkZ);

// Marca la posición del usuario (chunkX, chunkZ).
void viz_setUser(int chunkX, int chunkZ);

// Finaliza y guarda la imagen compuesta.
void viz_render(void);

#endif // VISUALIZER_H