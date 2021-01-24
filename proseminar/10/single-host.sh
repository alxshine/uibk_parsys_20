#!/bin/bash

for p in {1,2,4,8};
do
  for bin in {"pi","matrix"}
  do
    qsub -j y -q std.q -cwd -pe openmpi-${p}perhost $p -N $bin-$p -o $bin-$p.log << EOF
`pwd`/${bin}-multinode -nl 1 --numPUs=$p
EOF
  done
done
