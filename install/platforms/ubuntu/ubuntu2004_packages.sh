#!/bin/bash

#####################################################
# Ubuntu setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Author: Janne Beate Bakeng, SINTEF Medical Technology
# Date:   2012.05.19
#         2014.09.09
#
# Description:
#   Install all packages needed to:
#     * Develop in C++/Eclipse
#     * Run CustusX
#     * Several nice-to-have utilities.
#
#
#####################################################

#Ubuntu 20.04, with OpenCl for NVidia

sudo apt-get -y update
sudo apt-get -y upgrade
# developer stuff
sudo apt-get install -y build-essential opencl-headers libssl-dev 
sudo apt-get install -y qt4-dev-tools qt4-designer libboost-all-dev libcppunit-dev libglew-dev
sudo apt-get install -y qttools5-dev qttools5-dev-tools qttools5-doc qt5-doc libqt5webkit5-dev libqt5xmlpatterns5-dev libqt5x11extras5-dev qtmultimedia5-dev
sudo apt-get install -y ninja-build
# developer IDE's and apps
sudo apt-get install -y eclipse-cdt cmake-qt-gui qtcreator
# doxygen
sudo apt-get install -y doxygen gnuplot
# coverage testing
sudo apt-get install -y lcov
# compatibility pack for all 32 bit libs
sudo apt-get install -y ia32-libs
# source control systems
sudo apt-get install -y subversion cvs git-gui
# v4l
sudo apt-get install -y libv4l-dev
# used by jenkins
sudo apt-get install -y cppcheck sloccount vnc4server
# run x11 unit tests on jenkins user:
sudo apt-get install -y xvfb
# Dynamic Kernel Module Support (Ephiphan dependency)
sudo apt-get install -y dkms
# python libs
sudo apt-get install -y python-pip libxml2-dev libxslt-dev python-numpy
sudo easy_install lxml
sudo easy_install paramiko
#used by IMFusion
sudo apt-get install -y libcrypto++-dev libnlopt0

# utilities
sudo apt-get install -y synaptic aptitude apt-file dia gimp mesa-utils
# vlc
sudo apt-get install -y vlc browser-plugin-vlc
# medical image processing app
sudo apt-get install -y itksnap
# webcams
sudo apt-get install -y guvcview cheese qv4l2
# unity tweak tool
sudo apt-get install -y compizconfig-settings-manager
# vpn cisco plugin
sudo apt-get install -y network-manager-vpnc-gnome
# connect via ssh (also remember to enable firewall ufw and open port 22)
sudo apt-get install -y openssh-server
# Install pycrypto
#sudo pip install pycrypto future

#Cmake and Qt
sudo apt-get install -y cmake cmake-curses-gui cmake-qt-gui cmake-qt-gui qt5-default qtmultimedia5-dev qttools5-dev libqt5x11extras5-dev libqt5xmlpatterns5-dev
sudo apt-get install -y libxt-dev

#OpenCL
sudo apt install -y nvidia-opencl-dev

# For OpenCL (fails in OpenCLUtilityLibrary)
# Need to install proprietary NVIDIA drivers in "Additional Drivers" in OS Settings
# Got 2 chioces: binary driver 340.108 and driver metapackage nvidia-driver-390
# Tried NVIDIA binary driver 340.108 first, with and without nvidia-modprobe (no reboot)
# Then tried nvidia-driver-390. This cause VTK cmake to fail, and removing build folder didn't help
#sudo apt-get -y install nvidia-modprobe

#Possible fix to missing /usr/lib/x86_64-linux-gnu/libEGL.so
# sudo ln /usr/lib/x86_64-linux-gnu/libGL.so.1.7.0 /usr/lib/x86_64-linux-gnu/libGL.so
# https://askubuntu.com/questions/616065/the-imported-target-qt5gui-references-the-file-usr-lib-x86-64-linux-gnu-li

#For the new Acer (2021) machines it seems like the latest NVIDIA driver (470) works fine

#Needed to reinstall Boost, cppunit and glew:
sudo apt-get install -y libboost-all-dev libcppunit-dev libglew-dev

#Needed to reinstall Boost, cppunit, glew, vlc:
sudo apt-get install -y libboost-all-dev libcppunit-dev libglew-dev vlc

#For running tests: issue with missing lxml
sudo apt-get install -y python3-lxml

#For enabling vlc integration, and showing the H.264 encoded video from VLC
sudo apt install -y vlc-plugin-access-extra gstreamer1.0-libav
