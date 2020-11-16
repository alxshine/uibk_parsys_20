# Build this image:  docker build -t mpi .
#
# this is a modernized version of https://github.com/oweidner/docker.openmpi

FROM ubuntu:20.04

ENV USER mpi
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Amsterdam

RUN apt-get update -y && \
    # apt-get install -y --no-install-recommends sudo apt-utils && \
    apt-get install -y --no-install-recommends wget openssh-server mosh \
    gcc g++ gdb gfortran make valgrind \
    libopenmpi-dev openmpi-bin openmpi-common openmpi-doc binutils

# ------------------------------------------------------------
# Add an 'mpi' user
# ------------------------------------------------------------

RUN adduser --disabled-password --gecos "" ${USER} && \
    echo "${USER} ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# ------------------------------------------------------------
# Set-Up SSH with our Github deploy key
# SSHD config based on https://github.com/coralhq/docker-sshd
# ------------------------------------------------------------

RUN mkdir /var/run/sshd \
    && echo 'AuthorizedKeysFile %h/.ssh/authorized_keys' >> /etc/ssh/sshd_config \
    && sed -i 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' /etc/pam.d/sshd \
    && sed -ri 's/^PermitRootLogin\s+.*/PermitRootLogin yes/' /etc/ssh/sshd_config \
    && sed -ri 's/UsePAM yes/#UsePAM yes/g' /etc/ssh/sshd_config


# ------------------------------------------------------------
# Configure OpenMPI
# Taken from https://github.com/oweidner/docker.openmpi
# ------------------------------------------------------------

USER root

RUN rm -fr ${HOME}/.openmpi && mkdir -p ${HOME}/.openmpi
ADD default-mca-params.conf ${HOME}/.openmpi/mca-params.conf
RUN chown -R ${USER}:${USER} ${HOME}/.openmpi

COPY entry_point.sh /bin/entry_point.sh
CMD entry_point.sh
EXPOSE 22