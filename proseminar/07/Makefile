flags=-O2 -std=c99

all: stencil_mpi hybrid_stencil

stencil_mpi: mpi_stencil_2d.c base.c
	mpicc ${flags} $^ -fopenmp -o $@

hybrid_stencil: mpi_omp_hybrid.c base.c
	mpicc ${flags} $^ -fopenmp -o $@

.PHONY: run, launch, clean
run: stencil_mpi
	mpirun -n 8 --oversubscribe $< 4 2

launch: stencil_mpi
	rm -f *.log
	qsub launch launch.script && watch qstat

clean:
	rm -f stencil_mpi core* *.log
