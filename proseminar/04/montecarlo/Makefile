flags=-O2 -std=c99

all: pi_seq pi_mpi

pi_seq: pi_seq.c
	$(CC) $(flags) $< -o $@ -lm

pi_mpi: pi_mpi.c
	mpicc $(flags) $< -o $@ -lm

.PHONY: clean
clean:
	rm -f pi_seq pi_mpi