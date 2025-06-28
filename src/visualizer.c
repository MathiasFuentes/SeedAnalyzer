#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "finders.h"
#include "extra.h"
#include "visualizer.h"
#include "search_cli.h"    // leerOpcion(), presioneEnterParaContinuar()
#include "util.h"          // allocCache(), genBiomes(), initBiomeColors(), biomesToImage()
#include "criteria.h"      // criterioBusqueda, ID
#include "list.h"          // list_index()

// internal state
static char          g_outPath[256];
static int           g_originX, g_originZ;
static unsigned      g_pixscale;
static int           g_width, g_height;
static unsigned char *g_pixels;
static unsigned char  g_biomeColors[256][3];

// ----------------------------------------------------------------
// viz_init
// ----------------------------------------------------------------
void viz_init(const char *outImagePath,
              int originX,
              int originZ,
              unsigned pixscale,
              int widthChunks,
              int heightChunks)
{
    // build .ppm filename
    const char *ext = strrchr(outImagePath, '.');
    if (ext && strcmp(ext, ".png") == 0) {
        snprintf(g_outPath, sizeof(g_outPath),
                 "%.*s.ppm",
                 (int)(ext - outImagePath),
                 outImagePath);
    } else {
        strncpy(g_outPath, outImagePath, sizeof(g_outPath) - 1);
        g_outPath[sizeof(g_outPath)-1] = '\0';
    }

    g_originX  = originX;
    g_originZ  = originZ;
    g_pixscale = pixscale;
    g_width    = widthChunks  * pixscale;
    g_height   = heightChunks * pixscale;

    // allocate pixel buffer
    g_pixels = malloc((size_t)g_width * g_height * 3);
    if (!g_pixels) {
        fprintf(stderr, "[viz] alloc failed\n");
        exit(1);
    }

    // load the official Cubiomes palette
    initBiomeColors(g_biomeColors);
}

// ----------------------------------------------------------------
// viz_drawBiomes
// ----------------------------------------------------------------
void viz_drawBiomes(const int *cache)
{
    biomesToImage(
      g_pixels,
      (unsigned char (*)[3])g_biomeColors,
      cache,
      g_width / g_pixscale,
      g_height / g_pixscale,
      g_pixscale,
      1  // flip vertically
    );
}

// ----------------------------------------------------------------
// draw a little X cross
// ----------------------------------------------------------------
static void draw_cross(int px, int py,
                       unsigned char R,
                       unsigned char G,
                       unsigned char B)
{
    for (int d = -2; d <= 2; ++d) {
        int x1 = px + d, y1 = py + d;
        int x2 = px + d, y2 = py - d;
        if (x1>=0 && x1<g_width && y1>=0 && y1<g_height) {
            size_t idx = ((size_t)y1 * g_width + x1)*3;
            g_pixels[idx+0] = R;
            g_pixels[idx+1] = G;
            g_pixels[idx+2] = B;
        }
        if (x2>=0 && x2<g_width && y2>=0 && y2<g_height) {
            size_t idx = ((size_t)y2 * g_width + x2)*3;
            g_pixels[idx+0] = R;
            g_pixels[idx+1] = G;
            g_pixels[idx+2] = B;
        }
    }
}

// ----------------------------------------------------------------
// viz_addInterest / viz_setUser
// ----------------------------------------------------------------
void viz_addInterest(int chunkX, int chunkZ)
{
    int px = (chunkX - g_originX) * g_pixscale + g_pixscale/2;
    int py = (chunkZ - g_originZ) * g_pixscale + g_pixscale/2;
    draw_cross(px, py, 255, 0, 0);
}

void viz_setUser(int chunkX, int chunkZ)
{
    int px = (chunkX - g_originX) * g_pixscale + g_pixscale/2;
    int py = (chunkZ - g_originZ) * g_pixscale + g_pixscale/2;
    draw_cross(px, py, 0, 0, 255);
}

// ----------------------------------------------------------------
// viz_render
// ----------------------------------------------------------------
void viz_render(void)
{
    FILE *f = fopen(g_outPath, "wb");
    if (!f) {
        perror("[viz] fopen");
        exit(1);
    }
    // P6 header
    fprintf(f, "P6\n%d %d\n255\n", g_width, g_height);
    fwrite(g_pixels, 1, (size_t)g_width * g_height * 3, f);
    fclose(f);
    free(g_pixels);
    g_pixels = NULL;
    printf("Imagen guardada como %s\n", g_outPath);
    puts("Sólo debes ejecutar 'visor.py' y listo!! :D");

}

// ----------------------------------------------------------------
// visualizer2d
// ----------------------------------------------------------------
void visualizer2d(criterioBusqueda *c,
                  uint64_t seed,
                  RegionResult *arr,
                  int count)
{
    if (count <= 0) {
        puts("No hay zonas para visualizar. Ejecuta primero la búsqueda.");
        presioneEnterParaContinuar();
        return;
    }

    // list up to 15 candidates
    int n = count < 15 ? count : 15;
    puts("Elige una zona para visualizar:");
    for (int i = 0; i < n; ++i) {
        printf(" [%2d] x=%5d, z=%5d  dist=%.1f\n",
               i+1, arr[i].x0, arr[i].z0, arr[i].distance);
    }
    printf("Selecciona (1-%d) o 0 para cancelar: ", n);
    int sel = leerOpcion(0, n);
    if (sel == 0) {
        puts("Visualización cancelada.");
        presioneEnterParaContinuar();
        return;
    }
    RegionResult *r = &arr[sel-1];

    const unsigned pixscale = 4;
    const int BASE    = 64;
    const int MARGIN  = 10;
    const int CANW    = BASE + 2 * MARGIN;
    const int CANH    = CANW;

    // 1) init canvas around chosen zone + margin
    viz_init("zona_seleccionada.ppm",
             r->x0 - MARGIN,
             r->z0 - MARGIN,
             pixscale,
             CANW, CANH);

    // 2) regen biomes
    Generator gen;
    setupGenerator(&gen, MC_1_16, 0);
    applySeed(&gen, DIM_OVERWORLD, seed);

    // note: Range = {unitSize, startX, startZ, widthChunks, heightChunks, biomeLayer, structures}
    Range range = { 16, r->x0 - MARGIN, r->z0 - MARGIN, CANW, CANH, 0, 1 };
    int *cache = allocCache(&gen, range);
    genBiomes(&gen, cache, range);

    // 3) draw it
    viz_drawBiomes(cache);

    // 4) markers: user + chosen + each required biome/structure
    if (list_size(c->coordenadasIniciales) > 0) {
        int *uc = list_index(c->coordenadasIniciales, 0);
        viz_setUser(uc[0], uc[1]);
    }
    viz_addInterest(r->x0, r->z0);
    // highlight first occurrence of each required biome
    for (int i = 0; i < list_size(c->biomasRequeridos); ++i) {
        ID *bid = list_index(c->biomasRequeridos, i);
        int bioId = biome_name_to_id(bid->nombreCub);
        bool marked = false;
        for (int z = 0; z < BASE && !marked; ++z)
        for (int x = 0; x < BASE && !marked; ++x) {
            if (cache[z * BASE + x] == bioId) {
                viz_addInterest(r->x0 + x, r->z0 + z);
                marked = true;
            }
        }
    }
    // highlight each required structure
    for (int i = 0; i < list_size(c->estructurasRequeridas); ++i) {
        ID *sid = list_index(c->estructurasRequeridas, i);
        int st = struct_name_to_type(sid->nombreCub);
        bool marked = false;
        for (int z = 0; z < BASE && !marked; ++z)
        for (int x = 0; x < BASE && !marked; ++x) {
            if (isViableStructurePos(st, &gen,
                 r->x0 + x, r->z0 + z, DIM_OVERWORLD))
            {
                viz_addInterest(r->x0 + x, r->z0 + z);
                marked = true;
            }
        }
    }

    free(cache);

    // 5) render
    viz_render();
    presioneEnterParaContinuar();
}
