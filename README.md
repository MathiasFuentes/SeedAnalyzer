## SEED ANALYZER

---
## ¡Explora mundos en Minecraft, de una forma optima y divertida!

Seed Analyzer es una herramienta de consola diseñada para encontrar las mejores regiones posibles dentro de mundos generados por semillas (seeds) de Minecraft, basándose en criterios personalizados definidos por el usuario.

Ya sea que estés buscando una aldea cerca de un bioma específico, un océano junto a montañas, o simplemente la zona más cercana al spawn con tus estructuras favoritas, Seed Analyzer automatiza la búsqueda y te entrega los resultados óptimos de forma rápida y ordenada.

---
## ¿Qué hace Seed Analyzer?

-Te guía paso a paso con un menú interactivo.

-Permite seleccionar biomas, estructuras, coordenadas y rangos de búsqueda.

-Utiliza la librería Cubiomes para simular la generación de mundo como lo haría Minecraft.

-Calcula y selecciona la región más óptima en base a distancia al punto inicial.

-Genera un archivo con los resultados y permite su visualización.

---

##  Cómo compilar y ejecutar

---
### Requisitos previos:

-Tener instalado [Visual Studio Code](https://code.visualstudio.com/)..

-Instalar la extensión **C/C++** (Microsoft).

-Tener instalda la extencion Makefile (para que al momento de compilar sea mas corto).

-Tener instalada la librería Cubiomes (ver instrucciones en su repositorio oficial).

-Para la visualización 3D, necesitas Python con matplotlib instalado.

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
## Funcionalidades principales

 -Menú principal con submenús funcionales.

 -Creación de criterios de búsqueda:

 -Biomas deseados

 -Estructuras deseadas

 -Coordenadas iniciales

 -Rango en chunks

 -Guardado de criterios en archivo JSON 
    ```
    criteria.json.
    ```

 -Visualización de resumen de criterios seleccionados.

 -Carga de criterios desde archivo JSON.

 -Ejecución real del análisis con Cubiomes.

 -Ranking de zonas candidatas y selección óptima.

 -Visualización 3D del resultado con 
    ```
    plot.py.
    ```
---
## Diseño del sistema y estructura de datos

 -Listas enlazadas para almacenar dinámicamente biomas, estructuras y coordenadas.

 -Tablas Hash para mapear nombres legibles a códigos internos de Cubiomes.

 -Montículo mínimo (Heap) para ordenar regiones candidatas por distancia.
s
 -Árbol KD (KD-Tree) para búsquedas espaciales rápidas.

 -Cache LRU para evitar recomputar regiones superpuestas.

---
## Ejemplo de uso
     ```
    ===== SeedAnalyzer Menu =====
 1. Crear nuevos criterios de búsqueda
 2. Cargar criterios existentes
 3. Realizar búsqueda con criterios actuales
 4. Visualizar Resultados
 5. Salir del programa
 =============================

Seleccione una opción: 1
  ```
Luego se te guiará paso a paso por submenús para ingresar tus criterios.

---

