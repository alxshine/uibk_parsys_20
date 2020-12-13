flags=-O2 -std=c99

all: mpi_stencil hybrid_stencil

mpi_stencil: mpi_stencil_2d.c base.c
	mpicc ${flags} $^ -fopenmp -o $@

hybrid_stencil: mpi_omp_hybrid.c base.c
	mpicc ${flags} $^ -fopenmp -o $@

.PHONY: run, launch, clean
run: mpi_stencil
	mpirun -n 8 --oversubscribe $< 4 2

launch: mpi_stencil
	rm -f *.log
	qsub launch launch.script && watch qstat

clean:
	rm -f mpi_stencil hybrid_stencil core* *.log
