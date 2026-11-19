# Evidencias de ejecución

## Compilación

```
g++ -std=c++11 -Wall -Wextra -static -o sitrac.exe src/*.cpp
```

g++ 16.2 (MinGW-w64): **0 errores, 0 advertencias**.

## Pruebas reproducibles (20 de 20 pasan)

```
powershell -ExecutionPolicy Bypass -File tests/pruebas.ps1
```

El script ejecuta el programa real con entradas tecleadas, verifica la salida y
guarda **todo lo que el programa imprimió** en `tests/resultados.txt`.

| # | Qué se prueba | Qué se verifica |
|---|---|---|
| P01 (CP-01) | Ciudad `Bogota` registrada dos veces | Rechaza el duplicado; el listado sigue con 1 ciudad |
| P02 (CP-02) | Eliminar pasajero inexistente | Informa y la lista queda intacta |
| P03 (CP-03) | Atender y ver frente con la cola vacía | Mensajes de cola vacía, sin fallar |
| P04 (CP-04) | Conexión con distancia `-5` | Se rechaza; no se crea la conexión |
| P05 (CP-05) | Eliminar ciudad con 2 conexiones | Quita las 2 conexiones; la vecina queda sin conexiones |
| P06 (CP-06) | `abc` como opción y `hola` como ID | Avisa y recupera el menú; el programa termina bien |
| P07 | ID de pasajero repetido | Se rechaza; sigue habiendo 1 pasajero |
| P08 | Encolar entre ciudades sin ruta | Solicitud rechazada; cola vacía |
| P09 | ABB con 50, 30, 70, 20, 40 | Inorden `20 30 40 50 70`, preorden, postorden y altura 3 |
| P10 | ABB: borrar 30 (dos hijos) | Inorden correcto, lista y árbol con 4 pasajeros, altura 3 |
| P11 | BFS desde A | Niveles 0, 1, 1, 2, 3 en el orden esperado |
| P12 | DFS desde A (con ciudad aislada) | Orden A, B, D, C, E; alcanza 5 de 6 |
| P13 | Dijkstra A → E | `A -> B -> D -> E (8 km)`; hacia ciudad aislada informa que no hay ruta |
| P14 | Atender y deshacer | El viaje vuelve al frente de la cola; con historial vacío informa |
| P15 | Guardar y cargar en otra ejecución | Restaura ciudades, conexión 12.5 km, pasajero, cola y ABB |
| P16 | Archivo corrupto (conexión a ciudad inexistente) | Rechaza indicando la línea; datos actuales intactos |
| P17 | Archivo con basura | `no es un archivo SITRAC valido` |
| P18 | Archivo inexistente | `no se pudo abrir`, el programa sigue |
| P19 | Opciones inválidas (menú principal y submenú) | Mensajes con el rango correcto |
| P20 | Distancia `abc` y entrada que se acaba | Rechaza; termina con `[Fin de la entrada]` sin colgarse |

## Verificación adicional (no incluida en el repositorio)

Se escribió una prueba de estrés aparte que hizo miles de operaciones aleatorias
sobre cada estructura y las comparó contra una referencia sencilla:

- **Lista y ABB:** 30 rondas × 600 operaciones; siempre contienen los mismos pasajeros, `inorden` siempre sale ordenado, y se probaron borrados de hoja, un hijo, dos hijos y raíz, y el caso degenerado (IDs ordenados, altura = n).
- **Cola:** FIFO correcta con encolar, encolar al frente, atender y eliminar solicitudes de un pasajero; el puntero `fin` nunca queda colgando.
- **Grafo:** tras cada operación aleatoria no hay aristas hacia ciudades borradas y todas son simétricas; BFS/DFS alcanzan exactamente el componente conexo correcto; Dijkstra coincide con Floyd-Warshall y el camino impreso suma el costo informado.
- **Persistencia:** guardar y cargar conserva el orden de la cola y de la pila y reconstruye el ABB; 20 variantes de archivo dañado se rechazan sin alterar los datos.
- **Memoria:** contando cada `new` y `delete`, al terminar quedaron **0 bloques sin liberar**.
- Se comprobó que la prueba detecta errores: al introducir bugs a propósito (no quitar aristas al borrar ciudad, no borrar el sucesor en el ABB) falló miles de veces.

## Capturas de pantalla

Las capturas `tests/1.png`, `2.png`, `4.png` y `6.png` son de la versión del
taller del 12 de noviembre (menú de 8 opciones). Para la sustentación conviene
tomar capturas nuevas con la versión actual (menú de 10 opciones).
