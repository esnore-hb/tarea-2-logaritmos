CXX := g++
FLAGS := -Wall -Wextra -Wpedantic -O3 -o

DATA_GENERATOR := ./data/data-generator

.PHONY: all

all:
	$(CXX) $(DATA_GENERATOR).cpp $(FLAGS) $(DATA_GENERATOR).out
	bash $(DATA_GENERATOR).sh

clean:
	find . -type f \( -name "*.out" -o -name "*.data" \) -delete