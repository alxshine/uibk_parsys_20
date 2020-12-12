flags=-O2 -std=c99

all: stencil_mpi

stencil_mpi: mpi_stencil_2d.c base.c
	mpicc ${flags} $^ -o $@

run: stencil_mpi
	mpirun -n 8 --oversubscribe $< 200 8

.PHONY: clean
clean:
	rm -f stencil_mpi core* *.log