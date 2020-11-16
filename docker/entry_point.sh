#!/bin/bash
# Taken from https://github.com/coralhq/docker-sshd

SSH=/home/mpi/.ssh
mkdir -p $SSH
chmod 700 $SSH

authorized_keys=$SSH/authorized_keys

for user in $GITHUB_USERS; do
  echo "Getting ${user}'s public key from GitHub"
  wget --no-check-certificate "https://github.com/${user}.keys" -O $authorized_keys
done
echo "" >> $authorized_keys
chmod 600 $authorized_keys
chown mpi:mpi -R $SSH
printf "Authorized keys file: %s\n" $authorized_keys

echo "Starting SSH daemon"
/usr/sbin/sshd -D