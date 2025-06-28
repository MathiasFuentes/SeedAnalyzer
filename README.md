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

- Para la visualización 2D, necesitas Python con matplotlib instalado.

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

---
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

---
  ## Submenús y sus interacciones en cada opcion

  ### Case 1: 
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
---

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
   
---

  ### Case 2:  
  ```
  ====== Cargar Criterios Recibidos ======
  Estos son algunos ejemplos de archivos .json:
   --> criteria_montana.json
   --> criteria_oceano.json
   --> criteria_village_bioma.json

  Seleccione una opcion (0,1)
  1. Guardar criterios actuales en un archivo .json
  0. ---> Volver al menu principal
  ========================================

  Seleccione una opcion: 
  ```

  ---
  ## Características del Case 2:

  Esta opción permite al usuario guardar los criterios de búsqueda que definió previamente (en la opción 1) en un archivo 
    ```
    .json. 
    ```
    
  De esta manera, los criterios quedan registrados para ser reutilizados más adelante o compartidos.

  ## ¿Qué se guarda?
  
  - Biomas deseados
    
  - Estructuras deseadas

  - Coordenadas iniciales

  - Rango de búsqueda

Todo esto se escribe en formato JSON legible y reutilizable.

  ## Requisito

  Antes de guardar, el usuario debe haber definido sus criterios. Si aún no lo ha hecho, el sistema mostrará el siguiente mensaje:
  
  ```
  Primero debes definir tus criterios antes de poder guardarlos :)
  ```

  ## Ejemplo de archivo generado: 

  ```
  {
  "biomas": ["ocean", "plains"],
  "estructuras": ["village", "ruined_portal"],
  "coordenadas": [100, 200],
  "radio": 64
}

  ```
---


  ### Case 3: 
  ```
  ====== Realizar Búsqueda de la Zona =====
 1. Iniciar búsqueda con criterios actuales
 2. Modificar semilla de búsqueda
 3. Ver resultados de la búsqueda
 4. Volver al menú principal
  ```
---
## Interaciones Case 3:

### 1. Iniciar búsqueda con criterios actuales

- Ejecuta la función
    ```
    realizarBusqueda(c, gen, seed).
    ```
 - Toma los criterios definidos (biomas, estructuras, coordenadas, radio).

 - Utiliza la librería Cubiomes para simular la generación del mundo.

 - Divide el área de chunks en subregiones y evalúa cuáles cumplen los criterios.

 - Guarda las zonas candidatas y selecciona la más cercana al punto inicial.

 - Resultados se guardan en un arreglo que después sirve para ordenar las zonas candidatas por puntaje.
### 2. Modificar semilla de búsqueda

 - Permite ingresar una nueva semilla numérica
     ```
     uint64_t.
     ```

 - Esta semilla se pasa a
     ```
     setupGenerator()
     ```
    y afecta toda la simulación.

 - Asegura reproducibilidad: si usas la misma seed, obtendrás el mismo mundo.

### 3. Ver resultados de la búsqueda

 - Muestra por consola:

  - Coordenadas de la mejor región encontrada.

  - Distancia al punto inicial.

### 4. Volver al menú principal

 - Sale del submenú y retorna al menú principal.
 - La última búsqueda sigue vigente para su visualización.

---
## Interacciones Case 4

La función ejecutarOpcion4 abre el submenú de visualización, que permite al usuario generar una vista gráfica de la zona encontrada en la búsqueda:
```
void ejecutarOpcion4(criterioBusqueda *c,
                     KDTree *arbolito,
                     uint64_t *pseed,
                     RegionResult *lastResults,
                     int lastCount)
{
    int opcion;

    do {
        limpiarPantalla();
        menuOpcion4();
        printf("\nSeleccione una opción: ");
        opcion = leerOpcion(1, 4);
        switch(opcion)
        {
            case 1:
                // Lanza el visualizador 2D de Cubiomes
                visualizer2d(c, *pseed, lastResults, lastCount);
                break;
            case 2:
                // (Reservado para futura visualización 3D)
                puts("Volviendo al menú principal");
                return;
            default:
                puts("Opción no válida.");
                presioneEnterParaContinuar();
        }
    } while (opcion != 2);
}
```

## ¿Qué hace esta sección del programa?

### 1. Mostrar el submenú
Despeja la pantalla y presenta las opciones:

- Visualización 2D con Cubiomes

- Regresar al menú principal


### Opción 1: Visualizer2d
Invoca la función de visualización 2D, pasándole:

- El struct de criterios (c), para saber dónde está el punto de inicio y qué biomas/estructuras marcar.

- La semilla del mundo (*pseed).

- El array de resultados de la búsqueda (lastResults) y su tamaño (lastCount).

### El visualizador:

- Genera un mapa PPM (usando Cubiomes) de la región seleccionada con un pequeño margen.

- Pinta cada chunk con su color de bioma oficial.

- Dibuja una cruz azul en la posición inicial del usuario, cruces rojas en cada criterio cumplido y otra en la zona central.

- Guarda el resultado en zona_seleccionada.ppm y lo notifica al usuario.

- Para su visualización, simplemente se ejecuta el script de python visor.py.

### Opción 2 o cierre
Sale inmediatamente y retorna al menú principal.

### Control de flujo
El bucle se repite hasta que el usuario ingresa la opción de volver al menú principal (2) o cierra el submenú.


---
 ## Interacciones Case 5:

- Sale del programa.
---
## Contribuciones
- Matías Fuentes: Implementación de la estructura principal, uso de librerías  y funciones de agrupación por tipo.
- Bastián Guerra: Desarrollo del menú y lógica de búsqueda por variedad y nombre.
- Nestor Calderón: Apoyo en pruebas, validación de entradas y limpieza de datos.
## Agradecimientos
- Cubitect, por la librería de Cubiomes.
- PeterAlfredLee y Alanscut, por la librería de cJSON.
### Nota al profesor.
Profesor, si está leyendo esto, sabemos que dijimos que haríamos una visualización 3D, pero por cuestiones de tiempo, organización y cuestiones técnicas con cubiomes y las librerias, no pudimos implementarlo, sin embargo, pudimos implementar la visualización 2D en reemplazo de la 3D.
