# SEED ANALYZER

---
## ¡Explora mundos en Minecraft, de una forma optima y divertida!

Seed Analyzer es una herramienta de consola diseñada para encontrar las mejores regiones posibles dentro de mundos generados por semillas (seeds) de Minecraft, basándose en criterios personalizados definidos por el usuario.

Ya sea que estés buscando una aldea cerca de un bioma específico, un océano junto a montañas, o simplemente la zona más cercana al spawn con tus estructuras favoritas, Seed Analyzer automatiza la búsqueda y te entrega los resultados óptimos de forma rápida y ordenada.

---
## ¿Qué hace Seed Analyzer?

- Te guía paso a paso con un menú interactivo.

- Permite seleccionar biomas, estructuras, coordenadas y rangos de búsqueda.

- Utiliza la librería Cubiomes para simular la generación de mundo como lo haría Minecraft.

- Calcula y selecciona la región más óptima en base a distancia al punto inicial.

- Genera un archivo con los resultados y permite su visualización.

---

#  Cómo compilar y ejecutar

---
### Requisitos previos:

- Tener instalado [Visual Studio Code](https://code.visualstudio.com/)..

- Instalar la extensión **C/C++** (Microsoft).

- Tener instalda la extencion Makefile (para que al momento de compilar sea mas corto).

- Tener instalada la librería Cubiomes (ver instrucciones en su repositorio oficial).

- Para la visualización 3D, necesitas Python con matplotlib instalado.

---
### Pasos para compilar y ejecutar:

- Descarga y descomprime el archivo en una carpeta de tu elección.
  
- Abre el proyecto en Visual Studio Code
  
- Inicia Visual Studio Code.
    Selecciona **Archivo > Abrir carpeta...** y elige la carpeta donde descomprimiste el proyecto.
  
- Compila el código
    Abre el archivo principal ( **seed_analyzer.c** ).
    Abre la terminal integrada (**Terminal > Nueva terminal**).
    En la terminal, compila el programa con el siguiente comando:
        ```
        mingw32-make
        ```
- Ejecuta el programa,
    una vez compilado, puedes ejecutar con:
        ```
        ./seed_analyzer
        ```

---
# Funcionalidades principales

 - Menú principal con submenús funcionales.

 - Creación de criterios de búsqueda:

 - Biomas deseados

 - Estructuras deseadas

 - Coordenadas iniciales

 - Rango en chunks

 - Guardado de criterios en archivo JSON 
      ```
      criteria.json
      ```

 - Visualización de resumen de criterios seleccionados.

 - Carga de criterios desde archivo JSON.

 - Ejecución real del análisis con Cubiomes.

 - Ranking de zonas candidatas y selección óptima.

 - Visualización 3D del resultado con 
    ```
     plot.py.
    ```
---
# Diseño del sistema y estructura de datos

Seed Analyzer está diseñado sobre una arquitectura modular en C que aprovecha:

 - Listas enlazadas para almacenar dinámicamente biomas, estructuras y coordenadas.

 - Tablas Hash para mapear nombres legibles a códigos internos de Cubiomes.

 - Montículo mínimo (Heap) para ordenar regiones candidatas por distancia.

 - Árbol KD (KD-Tree) para búsquedas espaciales rápidas.

 - Cache LRU para evitar recomputar regiones superpuestas.

---
# Ejemplo de uso

 ## Menú Principal.
 
  ```
    ===== SeedAnalyzer Menu =====
 1. Crear nuevos criterios de búsqueda
 2. Cargar criterios existentes
 3. Realizar búsqueda con criterios actuales
 4. Visualizar Resultados
 5. Salir del programa
 =============================
  ```

  ### submenú case 1: 
  ```
    ===== Crear Nuevos Criterios =====
 1. Definir biomas deseados
 2. Definir estructuras deseadas
 3. Definir coordenadas específicas
 4. Definir rango de búsqueda
 5. Resumen de criterios seleccionados
 6. Volver al menú principal
 ===================================
  ```

## Interacciones Case 1:

### 1. Definir biomas deseados

- El sistema solicita al usuario ingresar hasta 3 biomas.

- Se almacenan en una lista dinámica 
    ```
     List *biomasRequeridos.
    ```

- Ejemplos válidos: 
    ```
     "plains", "mountains", "cherry_grove".
    ```
### 2. Definir estructuras deseadas

- Similar al anterior, permite seleccionar hasta 3 estructuras.

- Se almacenan en 
    ```
     List *estructurasRequeridas.
    ```

- Ejemplos: 
    ```
     "village", "stronghold", "ruined_portal".
    ```
### 3. Definir coordenadas específicas

- Solicita al usuario ingresar dos coordenadas (X, Z) iniciales.

- Se almacenan en 
    ```
     List *coordenadasIniciales.
    ```
- Estas se usan como punto de referencia para medir distancias.

### 4. Definir rango de búsqueda

 - Se pide un único valor entero, que representa el radio en chunks.

 - Se guarda en 
    ```
     List *radioBusquedaEnChunks.
    ```
 - El área total de escaneo será de tamaño: 
    ```
     N = 2 * radio + 1 
    ```
      chunks.

### 5. Resumen de criterios seleccionados

Imprime en pantalla un resumen con todos los valores actuales:

- Biomas ingresados

- Estructuras seleccionadas

- Coordenadas

- Radio

### 6. Volver al menú principal

 - Sale del submenú y retorna al menú principal.

  ### submenú case 3: 
  ```
  ====== Realizar Búsqueda de la Zona =====
 1. Iniciar búsqueda con criterios actuales
 2. Modificar semilla de búsqueda
 3. Ver resultados de la búsqueda
 4. Volver al menú principal
  ```

## Inetarciones Case 3:

### 1. Iniciar búsqueda con criterios actuales

- Ejecuta la función
    ```
    realizarBusqueda(c, gen, seed).
    ```
 - Toma los criterios definidos (biomas, estructuras, coordenadas, radio).

 - Utiliza la librería Cubiomes para simular la generación del mundo.

 - Divide el área de chunks en subregiones y evalúa cuáles cumplen los criterios.

 - Usa pthreads para paralelizar el proceso.

 - Guarda las zonas candidatas y selecciona la más cercana al punto inicial.

 - Resultados se guardan en un archivo como
      ```
      results.json.
      ```


  





   
 ## Interacciones Case 6:

- Sale del programa.
   
---

