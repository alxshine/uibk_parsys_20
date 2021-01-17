CC=gcc
CC_FLAGS=-O3 -g3
LD_FLAGS=-lm

.PHONEY: all
all: real

SRC=$(wildcard *.c)

real: $(SRC)
	@$(CC) $(CC_FLAGS) $(SRC) -o real $(LD_FLAGS)

.PHONEY: clean
clean:
	@rm real

.PHONEY: run
run: all
	@echo "Sequential:"
	@./real
	@echo
