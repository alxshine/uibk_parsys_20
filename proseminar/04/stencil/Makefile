flags=-O2 -std=c99

all: seq mpi omp

seq: seq_stencil_2d.c base.c
	gcc ${flags} $^ -o $@

mpi: mpi_stencil_2d.c base.c
	mpicc ${flags} $^ -o $@

omp: omp_stencil_2d.c base.c
	gcc $(flags) -fopenmp $^ -o $@

run: mpi
	mpirun -n 8 $< 200 8

.PHONY: clean
clean:
	rm -f seq mpi omp core* *.log