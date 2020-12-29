options := -std=c99 -Werror -Wall -pedantic -g

all: multiplication

multiplication: multiplication.c
	$(CC) $^ -fopenmp $(options) -o $@

.PHONY: clean
clean:
	rm -f multiplication