# Compilador y flags
CC = gcc
CFLAGS = -Iinclude -Wall -lm

# Archivos fuente
SRC = seed_analyzer.c \
      criteria.c \
      src/extra.c \
      src/cJSON.c \
      src/biomenoise.c \
      src/biomes.c \
      src/finders.c \
      src/generator.c \
      src/layers.c \
      src/noise.c \
      src/quadbase.c \
      src/util.c

# Nombre del ejecutable
OUT = seed_analyzer

# Regla principal
all:
	$(CC) $(SRC) $(CFLAGS) -o $(OUT) -Wl,--subsystem,console

# Limpiar ejecutable
clean:
	del $(OUT).exe 2>nul || true
