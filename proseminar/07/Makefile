flags=-O2 -std=c99

all: stencil_mpi

stencil_mpi: mpi_stencil_2d.c base.c
	mpicc ${flags} $^ -fopenmp -o $@

.PHONY: run, launch, clean
run: stencil_mpi
	mpirun -n 8 --oversubscribe $< 4000 8

launch: stencil_mpi
	rm -f *.log
	qsub launch launch.script && watch qstat

clean:
	rm -f stencil_mpi core* *.log
