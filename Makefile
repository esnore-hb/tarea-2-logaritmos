CXX := g++
FLAGS := -Wall -Wextra -Wpedantic -O3 -o

DATA_GENERATOR := data-generator
AVL_TEST := avl-test

.PHONY: all

all:
	make data-generator
	make avl
	make avl-test

data-generator:
	$(CXX) ./data/data-generator.cc $(FLAGS) ./data/data-generator.out
	bash ./scripts/data-generator.sh

avl:
	$(CXX) ./avl-test.cc $(FLAGS) ./avl-test.out

avl-test:
	bash ./scripts/avl-test.sh
	python ./avl-test.py

clean:
	find . -type f \( -name "*.out" -o -name "*.data" -o -name "*.results" -o -name "*.png" \) -delete
