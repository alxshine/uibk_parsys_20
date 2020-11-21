flags=-O2 -std=c99

all: pi_seq pi_mpi pi_omp

pi_seq: pi_seq.c
	$(CC) $(flags) $< -o $@ -lm

pi_mpi: pi_mpi.c
	mpicc $(flags) $< -o $@ -lm

pi_omp: pi_omp.c
	$(CC) $(flags) -fopenmp $< -o $@ -lm

.PHONY: clean
clean:
	rm -f pi_seq pi_mpi pi_omp