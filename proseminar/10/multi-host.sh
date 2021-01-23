#!/bin/bash

for p in {1,2,4,8};
do
  qsub -pe openmpi-${p}perhost $p -N "chapel-multinode-$p" -o "chapel-multinode-${p}.log" launch-multinode.script
done
