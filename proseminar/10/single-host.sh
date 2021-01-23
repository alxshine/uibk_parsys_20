#!/bin/bash

for p in {1,2,4,8};
do
  qsub -pe openmpi-${p}perhost $p -N "chapel-$p" -o "chapel-${p}.log" launch.script
done
