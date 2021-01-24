#!/bin/bash

for p in {1,2,4}
do
  let num_cpus=$p*8
  for bin in {"pi","matrix"}
  do
    qsub -j y -q std.q -cwd -pe openmpi-8perhost $num_cpus -N $bin-m$p -o ${bin}-multinode-${p}.log << EOF
`pwd`/${bin}_real -nl $p
EOF
  done
done
