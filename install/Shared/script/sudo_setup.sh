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

echo "Running install script..."
echo "Commands are run with sudo; thus " $USER " is added to the /etc/sudoers file."
echo "Enter root password:"
su -c 'echo "$USER       ALL=(ALL)       NOPASSWD: ALL" >> /etc/sudoers'
echo "sudo is updated."

