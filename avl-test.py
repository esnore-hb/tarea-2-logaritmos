"""Graficar los resultados obtenidos para la estructura AVL."""
import re
import matplotlib.pyplot as plt

# Archivos y etiquetas asociadas
experimentos = {
    "Aleatorio + Uniforme": "./results/avl-test-10.results",
    "Aleatorio + Sesgada": "./results/avl-test-10-b.results",
    "Ordenado + Uniforme": "./results/avl-test-10-o.results",
    "Ordenado + Sesgada": "./results/avl-test-10-bo.results"
}

insert_times = []
search_times = []
labels = []

for nombre, archivo in experimentos.items():
    with open(archivo, "r", encoding="utf-8") as f:
        contenido = f.read()

    # Extraer tiempo de inserción
    insert_match = re.search(r"\[INSERT\].*?:\s*(\d+)\s*us", contenido)

    # Extraer tiempo de búsqueda
    search_match = re.search(r"\[SEARCH\].*?:\s*(\d+)\s*us", contenido)

    if insert_match and search_match:
        insert_times.append(int(insert_match.group(1)))
        search_times.append(int(search_match.group(1)))
        labels.append(nombre)
    else:
        print(f"No fue posible leer correctamente {archivo}")

# Posiciones para las barras
x = range(len(labels))
width = 0.35

plt.figure(figsize=(10, 6))

# Barras de inserción
plt.bar(
    [i - width/2 for i in x],
    insert_times,
    width=width,
    label="Insert",
)

# Barras de búsqueda
plt.bar(
    [i + width/2 for i in x],
    search_times,
    width=width,
    label="Search",
)

plt.xticks(x, labels, rotation=15)
plt.ylabel("Tiempo (μs)")
plt.title("N=10")
plt.legend()
plt.grid(axis="y", linestyle="--", alpha=0.5)

plt.tight_layout()
plt.show()
