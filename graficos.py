#!/usr/bin/env python3
# ===========================================================================
# graficos.py  —  Graficos de linea comparativos de la Tarea 2 (AVL vs Splay).
#
# Lee los CSV producidos por experimentos_avl.cc y experimentos_splay.cc. Cada
# estructura escribe su propio archivo (sufijo .avl.csv / .splay.csv); este script
# los concatena por la columna `estructura` y dibuja AMBAS curvas juntas. Genera:
#   - graf_72_busqueda.png   : 7.2  ns/busqueda vs N, un panel por escenario
#   - graf_72_insercion.png  : 7.2  tiempo total de insercion vs N
#   - graf_73a_sequential.png: 7.3a tiempo total vs m + cotas m*log2(n/m) / m*log2(n)
#   - graf_73b_workingset.png: 7.3b ns/busqueda vs W + cotas log2(W) / log2(n)
#
# Uso:       python graficos.py
# Requiere:  pip install pandas matplotlib numpy
# ===========================================================================
import os
import math
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def cargar(base):
    """Concatena las filas AVL y Splay de un mismo experimento.

    `base` es el prefijo del CSV (p. ej. "base_scenarios"); se buscan los
    archivos <base>.avl.csv y <base>.splay.csv (y, por compatibilidad, el
    antiguo <base>.csv) y se unen en un solo DataFrame agrupable por estructura.
    """
    partes = []
    for path in (f"{base}.avl.csv", f"{base}.splay.csv", f"{base}.csv"):
        if os.path.exists(path):
            df = pd.read_csv(path)
            if not df.empty:
                partes.append(df)
    if not partes:
        print(f"(aviso) no hay CSVs para '{base}.*'")
        return None
    return pd.concat(partes, ignore_index=True)


def escalar_a(data, bound):
    """Escala 'bound' por una constante para que su suma iguale la de 'data'
    (recomendacion (b): cotas teoricas * constante para comparar visualmente)."""
    data = np.asarray(data, float)
    bound = np.asarray(bound, float)
    s = float(np.sum(bound))
    k = (float(np.sum(data)) / s) if s > 0 else 0.0
    return k * bound


def graf_base(df):
    if df is None:
        return
    titulos = {
        "a": "a) insercion aleatoria + busqueda uniforme",
        "b": "b) insercion aleatoria + busqueda sesgada",
        "c": "c) insercion ordenada + busqueda uniforme",
        "d": "d) insercion ordenada + busqueda sesgada",
    }
    fig, axs = plt.subplots(2, 2, figsize=(12, 8))
    for ax, esc in zip(axs.flat, ["a", "b", "c", "d"]):
        sub = df[df["escenario"] == esc]
        for estr, g in sub.groupby("estructura"):
            g = g.sort_values("N")
            ax.plot(g["N"], g["search_avg_ns"], marker="o", label=estr)
        ax.set_title(titulos[esc])
        ax.set_xscale("log", base=2)
        ax.set_xlabel("N (nodos)")
        ax.set_ylabel("ns por busqueda")
        ax.grid(True, alpha=0.3)
        ax.legend()
    fig.suptitle("7.2 Escenarios base — tiempo medio por busqueda")
    fig.tight_layout()
    fig.savefig("graf_72_busqueda.png", dpi=130)
    print("-> graf_72_busqueda.png")

    fig, ax = plt.subplots(figsize=(8, 5))
    for (esc, estr), g in df.groupby(["escenario", "estructura"]):
        g = g.sort_values("N")
        ax.plot(g["N"], g["insert_ns"] / 1e6, marker="o", label=f"{estr} {esc}")
    ax.set_xscale("log", base=2)
    ax.set_xlabel("N (nodos)")
    ax.set_ylabel("insercion total (ms)")
    ax.set_title("7.2 Tiempo total de insercion")
    ax.grid(True, alpha=0.3)
    ax.legend(fontsize=8)
    fig.tight_layout()
    fig.savefig("graf_72_insercion.png", dpi=130)
    print("-> graf_72_insercion.png")


def graf_seq(df):
    if df is None:
        return
    n = float(df["N"].iloc[0])
    fig, ax = plt.subplots(figsize=(8, 5))
    for estr, g in df.groupby("estructura"):
        g = g.sort_values("m")
        m = g["m"].to_numpy(float)
        t = g["total_ns"].to_numpy(float) / 1e6  # ms
        ax.plot(m, t, marker="o", label=f"{estr} (medido)")
        if str(estr).lower().startswith("avl"):
            b = m * np.log2(n)
            ax.plot(m, escalar_a(t, b), "--", alpha=0.7, label="~ m·log2(n)")
        else:  # Splay
            b = m * np.log2(np.maximum(n / np.maximum(m, 1.0), 2.0))
            ax.plot(m, escalar_a(t, b), "--", alpha=0.7, label="~ m·log2(n/m)")
    ax.set_xlabel("m (busquedas crecientes)")
    ax.set_ylabel("tiempo total (ms)")
    ax.set_title(f"7.3a Sequential Access  (n = 2^{int(round(math.log2(n)))})")
    ax.grid(True, alpha=0.3)
    ax.legend()
    fig.tight_layout()
    fig.savefig("graf_73a_sequential.png", dpi=130)
    print("-> graf_73a_sequential.png")


def graf_ws(df):
    if df is None:
        return
    n = float(df["N"].iloc[0])
    fig, ax = plt.subplots(figsize=(8, 5))
    for estr, g in df.groupby("estructura"):
        g = g.sort_values("W")
        W = g["W"].to_numpy(float)
        t = g["avg_ns"].to_numpy(float)  # ns por busqueda
        ax.plot(W, t, marker="o", label=f"{estr} (medido)")
        if str(estr).lower().startswith("avl"):
            b = np.full_like(W, math.log2(n))
            ax.plot(W, escalar_a(t, b), "--", alpha=0.7, label="~ log2(n) (cte)")
        else:  # Splay
            b = np.log2(np.maximum(W, 2.0))
            ax.plot(W, escalar_a(t, b), "--", alpha=0.7, label="~ log2(W)")
    ax.set_xscale("log")
    ax.set_xlabel("W (tamano del working set)")
    ax.set_ylabel("ns por busqueda")
    ax.set_title(f"7.3b Working Set  (n = 2^{int(round(math.log2(n)))})")
    ax.grid(True, alpha=0.3)
    ax.legend()
    fig.tight_layout()
    fig.savefig("graf_73b_workingset.png", dpi=130)
    print("-> graf_73b_workingset.png")


if __name__ == "__main__":
    graf_base(cargar("base_scenarios"))
    graf_seq(cargar("sequential_access"))
    graf_ws(cargar("working_set"))
    print("Listo.")
