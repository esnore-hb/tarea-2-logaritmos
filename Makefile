CXX := g++
FLAGS := -Wall -Wextra -Wpedantic -O3 -o

DATA_GENERATOR := data-generator
AVL_TEST := avl-test

.PHONY: all

data-generator:
	$(CXX) ./data/data-generator.cc $(FLAGS) ./data/data-generator.out
	bash ./scripts/data-generator.sh

avl:
	$(CXX) ./avl-test.cc $(FLAGS) ./avl-test.out

avl-test:
	bash ./scripts/avl-test.sh

clean:
	find . -type f \( -name "*.out" -o -name "*.data" -o -name "*.results" \) -delete
