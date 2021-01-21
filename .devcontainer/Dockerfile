# Build this image:  docker build -t mpi .
#
# this is a modernized version of https://github.com/oweidner/docker.openmpi

FROM chapel/chapel

# timezone already set in chapel Dockerfile

RUN apt-get update -y && \
    # apt-get install -y --no-install-recommends sudo apt-utils && \
    apt-get install -y --no-install-recommends \
    gcc g++ gdb gfortran make valgrind git \
    libopenmpi-dev openmpi-bin openmpi-common openmpi-doc binutils


RUN adduser --disabled-password --gecos '' dev
USER dev

# ------------------------------------------------------------
# Configure OpenMPI
# Taken from https://github.com/oweidner/docker.openmpi
# ------------------------------------------------------------

RUN rm -fr ${HOME}/.openmpi && mkdir -p ${HOME}/.openmpi
ADD default-mca-params.conf ${HOME}/.openmpi/mca-params.conf
RUN chown -R ${USER}:${USER} ${HOME}/.openmpi