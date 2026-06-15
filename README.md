# Tarea 2 — Árboles AVL vs Splay Trees (CC4102)

Implementación y comparación experimental de un **árbol AVL** y un **Splay Tree**.
El código mide los tiempos de inserción y búsqueda de los escenarios de la
sección **7.2** (4 configuraciones base) y de la sección **7.3** (Sequential
Access Theorem y Working Set Theorem), y genera gráficos comparativos AVL vs Splay.

## Estructura del proyecto

| Archivo | Qué es |
|---|---|
| `include/avl.cc` | Clase `AVL`: `Nodo`, rotaciones primitivas (sec. 3.2), inserción/búsqueda con balanceo. |
| `include/splay.cc` | Clase `SplayTree`: `Nodo`, rotaciones, `splay` iterativo, inserción/búsqueda. |
| `include/experimentos.hh` | Batería de experimentos (7.2 + 7.3) como **plantillas** sobre el tipo de árbol. Constantes del experimento (`c`, `λ`, semilla). |
| `experimentos_avl.cc` | `main()` que corre la batería con `AVL`. |
| `experimentos_splay.cc` | `main()` que corre la batería con `SplayTree`. |
| `graficos.py` | Genera los gráficos de línea comparativos a partir de los CSV. |
| `scripts/run_all.sh` | Compila y corre **todo** con un solo comando. |
| `data/data-generator.cc`, `avl-test.cc`, `avl-test.py` | Pipeline *legacy* (benchmark AVL-only de 7.2, formato `.results`). Opcional. |

> La parte AVL y la parte Splay comparten `include/experimentos.hh`, por lo que se
> ejecutan con **el mismo `c`, el mismo `λ` y el mismo dataset** (misma semilla),
> garantizando una comparación justa.

## Requisitos

- **Compilador C++17** (g++ o clang++).
- **Python 3** con `pip install -r requirements.txt` (necesita `pandas`, `matplotlib`, `numpy`).

### Nota para Windows / MinGW (importante)

Si compilas con un g++ moderno (p. ej. WinLibs UCRT) pero ejecutas dentro de
Git Bash / MSYS, en el `PATH` puede aparecer primero un `libstdc++-6.dll` **más
antiguo** (el de Git), y los binarios **segfaultean** por desajuste de ABI.
La solución es compilar **estático** con `-static` (ya incluido en el `Makefile`
y en `run_all.sh`). En Linux `-static` es inofensivo; si tu toolchain no lo
soporta, quítalo.

## Cómo ejecutar todo (recomendado)

```bash
# Corrida de prueba (2^20 ≈ 1 M, corre en segundos):
bash scripts/run_all.sh

# Corrida OFICIAL del informe (2^25, ~1 GB RAM, varios minutos):
EXP73=25 bash scripts/run_all.sh
```

Esto compila los dos drivers, los ejecuta y genera los gráficos. Equivalente con
`make` (si está disponible): `make` o `make EXP73=25`.

## Cómo ejecutar paso a paso

```bash
# 1) Compilar (usar -DEXP_73=25 para la corrida oficial)
g++ -O2 -std=c++17 -static -DEXP_73=20 experimentos_avl.cc   -o experimentos_avl.out
g++ -O2 -std=c++17 -static -DEXP_73=20 experimentos_splay.cc -o experimentos_splay.out

# 2) Ejecutar (escriben los CSV; el progreso va por stderr)
./experimentos_avl.out
./experimentos_splay.out

# 3) Graficar
python graficos.py
```

### Salidas

CSV (uno por estructura, se concatenan en el gráfico):
`base_scenarios.{avl,splay}.csv`, `sequential_access.{avl,splay}.csv`,
`working_set.{avl,splay}.csv`.

Gráficos:
- `graf_72_busqueda.png` — 7.2: ns por búsqueda vs N, un panel por escenario (a,b,c,d).
- `graf_72_insercion.png` — 7.2: tiempo total de inserción vs N.
- `graf_73a_sequential.png` — 7.3a: tiempo total vs m, con cotas `m·log₂(n/m)` (Splay) y `m·log₂(n)` (AVL).
- `graf_73b_workingset.png` — 7.3b: ns por búsqueda vs W, con cotas `log₂(W)` (Splay) y `log₂(n)` (AVL).

## Constantes del experimento (en `include/experimentos.hh`)

| Constante | Valor | Significado |
|---|---|---|
| `C` | `1` | `c ∈ {1..10}`; número de búsquedas `M = 10·c·N`. |
| `LAMBDA` | `0.01` | `λ` de la función sesgada `P(i) ∝ e^(−λi)` (sec. 7.1.2). |
| `SEMILLA` | fija | Semilla base; datasets **reproducibles** e idénticos para AVL y Splay. |
| `EXP_73` | `20` (macro) | Tamaño de 7.3 = `2^EXP_73`. **El informe pide 2^25:** compilar con `-DEXP_73=25` (no requiere editar el código). |

## Decisiones de implementación (resumen para el informe)

- **`splay` iterativo** (vector que guarda el camino raíz→nodo) en vez de recursivo,
  para no desbordar la pila con árboles de profundidad ~2^25.
- **Destructor iterativo del Splay** (pila explícita en el heap): tras una secuencia
  de accesos un Splay Tree puede degenerar en un camino de profundidad ~n, lo que
  haría stack-overflow con un destructor recursivo.
- Construido sobre las **rotaciones primitivas** de la sección 3.2; las dobles
  `zig_zig`/`zag_zag` (que el AVL no usa) son las que emplea el splay.
- **Duplicados ignorados** (semántica de conjunto) en ambas estructuras, para una
  comparación justa.
- **Experimentos como plantillas**: AVL y Splay comparten exactamente el mismo
  código de medición, misma `c`, mismo `λ` y misma semilla ⇒ mismos datos y mismo
  orden de inserción. En 7.3 el árbol se **reconstruye antes de cada `m`/`W`** para
  que cada medición sea independiente.
- **Anti dead-code**: las búsquedas acumulan en una variable `volatile` para que el
  compilador no las elimine.
