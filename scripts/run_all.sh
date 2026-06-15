#!/usr/bin/env bash
# ===========================================================================
# run_all.sh  —  Corre TODA la batería de experimentos de la Tarea 2 sin tocar
#                el código: compila los dos drivers (AVL y Splay), los ejecuta y
#                genera los gráficos comparativos.
#
# Uso:
#   bash scripts/run_all.sh            # corrida de prueba, 2^20 (segundos)
#   EXP73=25 bash scripts/run_all.sh   # corrida OFICIAL del informe, 2^25 (minutos, ~1 GB RAM)
#
# Variables de entorno opcionales:
#   EXP73  exponente de los experimentos 7.3 (default 20; usar 25 para el informe)
#   CXX    compilador C++ (default g++)
# ===========================================================================
set -euo pipefail
cd "$(dirname "$0")/.."   # raíz del repo

CXX="${CXX:-g++}"
EXP73="${EXP73:-20}"
# -static evita el conflicto de DLLs de libstdc++ en Windows/MinGW. En Linux es
# inofensivo; si tu toolchain no soporta -static, quítalo de FLAGS.
FLAGS=(-O2 -std=c++17 -static -Wall -Wextra "-DEXP_73=${EXP73}")

echo "== Compilando drivers (EXP_73=${EXP73}) =="
"$CXX" "${FLAGS[@]}" experimentos_avl.cc   -o experimentos_avl.out
"$CXX" "${FLAGS[@]}" experimentos_splay.cc -o experimentos_splay.out

echo "== Ejecutando experimentos AVL =="
./experimentos_avl.out

echo "== Ejecutando experimentos Splay =="
./experimentos_splay.out

echo "== Generando gráficos comparativos =="
python graficos.py

echo "== Listo. Gráficos: graf_72_busqueda.png, graf_72_insercion.png, graf_73a_sequential.png, graf_73b_workingset.png =="
