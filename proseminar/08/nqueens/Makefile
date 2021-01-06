options := -std=c99 -O2 -Werror -Wall -pedantic -g

all: nqueens

nqueens: nqueens.c
	$(CC) $^ -fopenmp $(options) -o $@

.PHONY: clean
clean:
	rm -f nqueens