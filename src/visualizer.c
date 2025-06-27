#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "visualizer.h"
#include "util.h"       // parseBiomeColors, biomesToImage

// Estado interno
static char g_outPath[256];
static int  g_originX, g_originZ;
static unsigned g_pixscale;
static int  g_width, g_height;
static unsigned char *g_pixels = NULL;
static unsigned char g_biomeColors[256][3];

void visualizer_option(){
    
}

void viz_init(const char *outImagePath,
              int originX, int originZ,
              unsigned pixscale)
{
    // Guardar configuración
    // Cambiamos extensión a .ppm si es .png
    const char *ext = strrchr(outImagePath, '.');
    if (ext && strcmp(ext, ".png")==0) {
        snprintf(g_outPath, sizeof(g_outPath),
                 "%.*s.ppm",
                 (int)(ext - outImagePath),
                 outImagePath);
    } else {
        strncpy(g_outPath, outImagePath, sizeof(g_outPath));
        g_outPath[sizeof(g_outPath)-1] = '\0';
    }

    g_originX = originX;
    g_originZ = originZ;
    g_pixscale = pixscale;
    g_width  = 64 * pixscale;
    g_height = 64 * pixscale;

    // Reserva buffer RGB
    g_pixels = malloc(g_width * g_height * 3);
    if (!g_pixels) {
        fprintf(stderr, "[viz] No hay memoria para imagen\n");
        exit(1);
    }

    // Carga paleta desde palette.txt
    FILE *f = fopen("palette.txt", "r");
    if (!f) {
        fprintf(stderr, "[viz] palette.txt no encontrado\n");
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz+1);
    fread(buf,1,sz,f);
    buf[sz]='\0';
    fclose(f);

    if (parseBiomeColors(g_biomeColors, buf)!=0) {
        fprintf(stderr,"[viz] Error parseando palette.txt\n");
        exit(1);
    }
    free(buf);
}

void viz_drawBiomes(const int *cache)
{
    biomesToImage(g_pixels,
                  (unsigned char(*)[3])g_biomeColors,
                  cache,
                  64,64,
                  g_pixscale,
                  1);
}

// Dibuja un cruce de 5px en (cx,cy)
static void draw_cross(int cx,int cy,
                       unsigned char r,
                       unsigned char g,
                       unsigned char b)
{
    for(int dy=-2;dy<=2;++dy){
        int y=cy+dy, x1=cx+dy, x2=cx-dy;
        if(x1>=0&&x1<g_width&&y>=0&&y<g_height){
            unsigned idx=(y*g_width+x1)*3;
            g_pixels[idx]=r;g_pixels[idx+1]=g;g_pixels[idx+2]=b;
        }
        if(x2>=0&&x2<g_width&&y>=0&&y<g_height){
            unsigned idx=(y*g_width+x2)*3;
            g_pixels[idx]=r;g_pixels[idx+1]=g;g_pixels[idx+2]=b;
        }
    }
}

void viz_addInterest(int chunkX,int chunkZ)
{
    int px=(chunkX-g_originX)*g_pixscale+g_pixscale/2;
    int pz=(chunkZ-g_originZ)*g_pixscale+g_pixscale/2;
    draw_cross(px,pz,255,0,0);
}

void viz_setUser(int chunkX,int chunkZ)
{
    int px=(chunkX-g_originX)*g_pixscale+g_pixscale/2;
    int pz=(chunkZ-g_originZ)*g_pixscale+g_pixscale/2;
    draw_cross(px,pz,0,0,255);
}

void viz_render(void)
{
    // Escribe PPM (P6)
    FILE *f = fopen(g_outPath,"wb");
    if (!f) { perror("[viz] fopen"); exit(1); }
    fprintf(f,"P6\n%d %d\n255\n", g_width, g_height);
    fwrite(g_pixels,1,g_width*g_height*3,f);
    fclose(f);
    free(g_pixels);
    printf("Imagen guardada en %s\n", g_outPath);
}
