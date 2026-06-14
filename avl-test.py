"""Graficar los resultados obtenidos para la estructura AVL."""

import re
from pathlib import Path

import matplotlib.pyplot as plt

# Valores de N a considerar
Ns = [10, 11, 12, 13, 14]

# Configuración de experimentos
experimentos = {
    "Aleatorio + Uniforme": "",
    "Aleatorio + Sesgada": "-b",
    "Ordenado + Uniforme": "-o",
    "Ordenado + Sesgada": "-bo",
}

# Estructuras para almacenar resultados
insert_data = {nombre: [] for nombre in experimentos}
search_data = {nombre: [] for nombre in experimentos}

for N in Ns:
    for nombre, sufijo in experimentos.items():
        archivo = Path(f"./results/avl-test-{N}{sufijo}.results")

        if not Path.exists(archivo):
            print(f"Advertencia: no existe {archivo}")
            insert_data[nombre].append(None)
            search_data[nombre].append(None)
            continue

        with Path.open(archivo, "r", encoding="utf-8") as f:
            contenido = f.read()

        insert_match = re.search(r"\[INSERT\].*?:\s*(\d+)\s*us", contenido)

        search_match = re.search(r"\[SEARCH\].*?:\s*(\d+)\s*us", contenido)

        if insert_match and search_match:
            insert_data[nombre].append(int(insert_match.group(1)))
            search_data[nombre].append(int(search_match.group(1)))
        else:
            print(f"No se pudieron extraer datos desde {archivo}")
            insert_data[nombre].append(None)
            search_data[nombre].append(None)

# n real de elementos
n_real = [2**N for N in Ns]

#
# Inserción: Θ(n log n) = Θ(N*2^N)
#
insert_teorica = [N * (2**N) for N in Ns]

#
# Búsqueda: Θ(M log n)
# M = 20N
# => Θ(20*2^N)
#
search_teorica = [20 * (2**N) for N in Ns]

# Escalar inserción
factor_insert = insert_data["Aleatorio + Uniforme"][-1] / insert_teorica[-1]

insert_teorica = [x * factor_insert for x in insert_teorica]


# Escalar búsqueda
factor_search = search_data["Aleatorio + Uniforme"][-1] / search_teorica[-1]

search_teorica = [x * factor_search for x in search_teorica]

#
# Gráfico de inserciones
#
plt.figure(figsize=(10, 6))

for nombre, tiempos in insert_data.items():
    plt.plot(Ns, tiempos, marker="o", linewidth=2, label=nombre)

plt.plot(Ns, insert_teorica, "--", linewidth=3, marker="s", label="Θ(N·2^N)")

plt.title("AVL - Inserción")
plt.xlabel("N (n = 2^N)")
plt.ylabel("Tiempo (μs)")
plt.xticks(Ns)
plt.grid(visible=True, linestyle="--", alpha=0.5)
plt.legend()

plt.tight_layout()
plt.savefig("avl-test-insert.png", dpi=300)
plt.close()


#
# Gráfico de búsquedas
#
plt.figure(figsize=(10, 6))

for nombre, tiempos in search_data.items():
    plt.plot(Ns, tiempos, marker="o", linewidth=2, label=nombre)

plt.plot(Ns, search_teorica, "--", linewidth=3, marker="s", label="Θ(20·2^N)")

plt.title("AVL - Búsqueda")
plt.xlabel("N (n = 2^N)")
plt.ylabel("Tiempo (μs)")
plt.xticks(Ns)
plt.grid(visible=True, linestyle="--", alpha=0.5)
plt.legend()

plt.tight_layout()
plt.savefig("avl-test-search.png", dpi=300)
plt.close()
