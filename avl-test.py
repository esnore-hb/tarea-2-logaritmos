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

#
# Gráfico de inserciones
#
plt.figure(figsize=(10, 6))

for nombre, tiempos in insert_data.items():
    plt.plot(Ns, tiempos, marker="o", linewidth=2, label=nombre)

plt.title("AVL - Tiempo de Inserción")
plt.xlabel("N")
plt.ylabel("Tiempo (μs)")
plt.xticks(Ns)
plt.grid(visible=True, linestyle="--", alpha=0.5)
plt.legend()

plt.tight_layout()
plt.show()


#
# Gráfico de búsquedas
#
plt.figure(figsize=(10, 6))

for nombre, tiempos in search_data.items():
    plt.plot(Ns, tiempos, marker="o", linewidth=2, label=nombre)

plt.title("AVL - Tiempo de Búsqueda")
plt.xlabel("N")
plt.ylabel("Tiempo (μs)")
plt.xticks(Ns)
plt.grid(visible=True, linestyle="--", alpha=0.5)
plt.legend()

plt.tight_layout()
plt.show()
