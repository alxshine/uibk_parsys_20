# Build this image:  docker build -t mpi .
#
# this is a modernized version of https://github.com/oweidner/docker.openmpi

FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Amsterdam

RUN apt-get update -y && \
    # apt-get install -y --no-install-recommends sudo apt-utils && \
    apt-get install -y --no-install-recommends wget openssh-server mosh \
    gcc g++ gdb gfortran make valgrind git \
    libopenmpi-dev openmpi-bin openmpi-common openmpi-doc binutils

# ------------------------------------------------------------
# Set-Up SSH with our Github deploy key
# SSHD config based on https://github.com/coralhq/docker-sshd
# ------------------------------------------------------------

RUN mkdir /var/run/sshd \
    && echo 'AuthorizedKeysFile %h/.ssh/authorized_keys' >> /etc/ssh/sshd_config \
    && sed -i 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' /etc/pam.d/sshd \
    && sed -ri 's/^PermitRootLogin\s+.*/PermitRootLogin yes/' /etc/ssh/sshd_config \
    && sed -ri 's/UsePAM yes/#UsePAM yes/g' /etc/ssh/sshd_config

RUN adduser --disabled-password --gecos '' dev
USER dev

# get necessary keys during build
ARG github_user
RUN test -n "$github_user"

RUN mkdir ${HOME}/.ssh
RUN chmod 700 ${HOME}/.ssh
RUN wget --no-check-certificate "https://github.com/${github_user}.keys" -O ${HOME}/.ssh/authorized_keys
RUN chmod 600 ${HOME}/.ssh/authorized_keys

# ------------------------------------------------------------
# Configure OpenMPI
# Taken from https://github.com/oweidner/docker.openmpi
# ------------------------------------------------------------

RUN rm -fr ${HOME}/.openmpi && mkdir -p ${HOME}/.openmpi
ADD default-mca-params.conf ${HOME}/.openmpi/mca-params.conf
RUN chown -R ${USER}:${USER} ${HOME}/.openmpi

USER root

CMD ["/usr/sbin/sshd", "-D"]
# CMD ["bash"]