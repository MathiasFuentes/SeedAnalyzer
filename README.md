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

-Tambien realiza la búsqueda usando pthreads para aprovechar múltiples núcleos del procesador.

-Calcula y selecciona la región más óptima en base a distancia al punto inicial.

-Genera un archivo con los resultados y permite visualización 3D.
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
---
### Pasos para compilar y ejecutar:

- Descarga y descomprime el archivo en una carpeta de tu elección.
- Abre el proyecto en Visual Studio Code
- Inicia Visual Studio Code.
    Selecciona **Archivo > Abrir carpeta...** y elige la carpeta donde descomprimiste el proyecto.
- Compila el código
    Abre el archivo principal (**seed_analyzer.c**).
    Abre la terminal integrada (**Terminal > Nueva terminal**).
    En la terminal, compila el programa con el siguiente comando:
        ```
        mingw32-make
        ```
- Ejecuta el programa
    Una vez compilado, puedes ejecutar la aplicación con:
        ```
        ./seed_analyzer
        ```

---

