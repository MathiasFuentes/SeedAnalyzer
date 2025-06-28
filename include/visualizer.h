#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <inttypes.h>
#include "criteria.h"    // typedef struct criterioBusqueda, ID
#include "search.h"      // typedef struct RegionResult

#ifdef __cplusplus
extern "C" {
#endif

// Initialize an expandable canvas.
//   outImagePath: where to write the PPM.
//   originX/Z: chunk at the upper‐left corner of the image.
//   pixscale: pixels per chunk.
//   widthChunks, heightChunks: size in chunks.
void viz_init(const char *outImagePath,
              int originX,
              int originZ,
              unsigned pixscale,
              int widthChunks,
              int heightChunks);

// Draw the biome map from the int‐array cache you filled with genBiomes+allocCache.
void viz_drawBiomes(const int *cache);

// Mark the user’s initial chunk (blue X).
void viz_setUser(int chunkX, int chunkZ);

// Mark a zone of interest (red X).
void viz_addInterest(int chunkX, int chunkZ);

// Write out the buffer as PPM and free it.
void viz_render(void);

// High-level 2D interactive visualizer.
//   c: your search criteria (for user coords, biomes, structures).
//   seed: the world seed.
//   arr/count: array of candidate RegionResult’s from your last search.
void visualizer2d(criterioBusqueda *c,
                  uint64_t seed,
                  RegionResult *arr,
                  int count);

#ifdef __cplusplus
}
#endif

#endif // VISUALIZER_H
