#!/bin/bash

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2011.08.04
#
# Description:
#
#	Allow user to use sudo.
#
#####################################################

#set -x
USER=`whoami`

echo "===== Installing sudo... ====="
echo "Commands are run with sudo; thus " $USER " is added to the /etc/sudoers file."
echo "Enter root password:"

set -x
su -c 'echo "$USER       ALL=(ALL)       NOPASSWD: ALL" >> /etc/sudoers'
set +x

echo "===== sudo is updated! ====="
echo ""
echo ""
