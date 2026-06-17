CXX := g++
# -static evita el conflicto de DLLs de libstdc++ en Windows/MinGW.
FLAGS := -O3 -std=c++17 -static -Wall -Wextra
# Tamaño de los experimentos 7.3: 2^EXP73. Oficial del informe: make EXP73=25
EXP73 := 20

.PHONY: all experimentos graficos clean legacy

# ---------------------------------------------------------------------------
# Pipeline PRINCIPAL: AVL vs Splay con configuración unificada (7.2 + 7.3).
#   make            -> compila, corre ambas estructuras y grafica (2^20, prueba)
#   make EXP73=25   -> corrida OFICIAL del informe (2^25, ~1 GB RAM, minutos)
# ---------------------------------------------------------------------------
all: experimentos graficos

experimentos:
	$(CXX) $(FLAGS) -DEXP_73=$(EXP73) experimentos_avl.cc   -o experimentos_avl.out
	$(CXX) $(FLAGS) -DEXP_73=$(EXP73) experimentos_splay.cc -o experimentos_splay.out
	./experimentos_avl.out
	./experimentos_splay.out

graficos:
	python graficos.py

clean:
	find . -type f \( -name "*.out" -o -name "*.data" -o -name "*.results" -o -name "*.png" -o -name "*.csv" \) -delete
