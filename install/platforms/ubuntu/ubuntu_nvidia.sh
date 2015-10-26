#!/bin/bash

#####################################################
# Ubuntu setup script
# Author: Christian Askeland, SINTEF Medical Technology
#         2014.09.09
#         2014.12.02
#
# Description:
#    Tweaks to make NVidia GeForce GTX 780 to work on Ubuntu 14.04
#    including openCL support
#
#####################################################

sudo update-pciids # causes lspci | grep -i nvidia  to report correct card

#Ubuntu 14.04 NVidia hacks - if driver is not present in "Additional drivers"
sudo add-apt-repository ppa:xorg-edgers/ppa -y
sudo apt-get -y update
sudo apt-get -y upgrade
#After this, you will see the drivers in System Settings -> Software update -> Additional Drivers

# install nvidia-modprobe in order to load the opencl driver automatically
# https://devtalk.nvidia.com/default/topic/734098/linux/-boinc-ubuntu-nvidia-no-usable-gpus-found-/
sudo apt-get -y install nvidia-modprobe

# needed to compile VTK on a Ubuntu 14.04 dated 2015-08+
sudo apt-get -y install libxnvctrl-dev 



