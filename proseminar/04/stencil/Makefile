flags=-g

all: seq mpi

seq: seq_stencil_2d.c base.c
	gcc ${flags} $^ -o $@

mpi: mpi_stencil_2d.c base.c
	mpicc ${flags} $^ -o $@

run: mpi
	mpirun -n 4 $<

.PHONY: clean
clean:
	rm -f seq mpi core* *.log