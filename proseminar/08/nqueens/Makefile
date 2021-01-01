options := -std=c++11 -Werror -Wall -pedantic -g

all: nqueens

nqueens: nqueens.cpp
	$(CXX) $^ -fopenmp $(options) -o $@

.PHONY: clean
clean:
	rm -f nqueens