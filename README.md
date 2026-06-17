# Tarea 2 — Árboles AVL vs Splay Trees (CC4102)

Implementación y comparación experimental de un **árbol AVL** y un **Splay Tree**.
El código mide los tiempos de inserción y búsqueda de 4 configuraciones base,
como también con Sequential Access Theorem y Working Set Theorem; Genera
gráficos comparativos AVL vs Splay.

## Estructura del proyecto

| Archivo | Qué es |
| --- | --- |
| `include/avl.cc` | Clase `AVL`: se implementan `Nodo`, rotaciones primitivas e inserción/búsqueda con balanceo. |
| `include/splay.cc` | Clase `SplayTree`: se implementan `Nodo`, rotaciones, `splay` iterativo, inserción/búsqueda. |
| `include/experimentos.hh` | Batería de experimentos como **plantillas** sobre el tipo de árbol. Constantes del experimento (`c`, `λ`, semilla). |
| `experimentos_avl.cc` | `main()` que corre la batería con `AVL`. |
| `experimentos_splay.cc` | `main()` que corre la batería con `SplayTree`. |
| `graficos.py` | Genera los gráficos de línea comparativos a partir de los CSV. |
| `scripts/run_all.sh` | Compila y corre **todo** con un solo comando. |

## Requisitos

- **Compilador C++17** (g++ o clang++).
- **Python 3** con `pip install -r requirements.txt`

> Se sugiere un ambiente de linux debido al compilador, como también operadores
> de terminal utilizados en los scripts.

## Cómo ejecutar todo

```bash
# N=2^20, corre en segundos:
bash scripts/run_all.sh

# N=2^25, corre hartos minutos:
EXP73=25 bash scripts/run_all.sh
```

Esto compila los archivos, los ejecuta y genera los gráficos. Equivalente con
`make` (si está disponible): `make` o `make EXP73=25`.

### Salidas

CSV (uno por estructura, se concatenan en el gráfico):
`base_scenarios.{avl,splay}.csv`, `sequential_access.{avl,splay}.csv`,
`working_set.{avl,splay}.csv`.

Gráficos:

- `graf_72_busqueda.png`: ns por búsqueda vs N, un panel por escenario (a,b,c,d).
- `graf_72_insercion.png`: tiempo total de inserción vs N.
- `graf_73a_sequential.png`: tiempo total vs m, con cotas `m·log₂(n/m)` (Splay) y `m·log₂(n)` (AVL).
- `graf_73b_workingset.png`: ns por búsqueda vs W, con cotas `log₂(W)` (Splay) y `log₂(n)` (AVL).

## Constantes del experimento

| Constante | Valor |
| --- | --- |
| `C` | `1` |
| `LAMBDA` | `0.01` |
| `SEMILLA` | fija |
| `EXP_73` | `20` o `25` |

## Decisiones de implementación

- **`splay` iterativo** (vector que guarda el camino raíz→nodo) en vez de recursivo,
  para no desbordar la pila con árboles de profundidad ~2^25.
- **Destructor iterativo del Splay** (pila explícita en el heap): tras una secuencia
  de accesos un Splay Tree puede degenerar en un camino de profundidad ~n, lo que
  haría stack-overflow con un destructor recursivo.
- **Duplicados ignorados** (semántica de conjunto) en ambas estructuras, para una
  comparación justa.
- **Experimentos como plantillas**: AVL y Splay comparten exactamente el mismo
  código de medición, misma `c`, mismo `λ` y misma semilla ⇒ mismos datos y mismo
  orden de inserción. El árbol se **reconstruye antes de cada `m`/`W`** para
  que cada medición sea independiente.
- **Anti dead-code**: las búsquedas acumulan en una variable `volatile` para que el
  compilador no las elimine.
