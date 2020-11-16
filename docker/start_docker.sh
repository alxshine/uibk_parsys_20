#!/bin/bash

GH_USER=alxshine
ROOT=$(cd .. && pwd) # alternatively hard-code this path

docker run --rm -it --name=mpi \
    -e GITHUB_USERS=${GH_USER} \
    --mount type=bind,src=${ROOT},dst=/home/mpi/scratch \
    -p 2222:22 mpi
