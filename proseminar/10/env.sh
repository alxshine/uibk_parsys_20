#!/bin/bash

module load openmpi/4.0.3
source chapel-1.23.0/util/setchplenv.bash
export CHPL_LAUNCHER=gasnetrun_mpi
export CHPL_COMM=gasnet
export CHPL_COMM_SUBSTRATE=mpi
