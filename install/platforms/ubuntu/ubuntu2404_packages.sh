#!/bin/bash

#####################################################
# Ubuntu setup script
#   Install all packages needed to:
#     * Develop in C++
#     * Run CustusX
#     * Several nice-to-have utilities.
#
#####################################################

#Ubuntu 24.04

sudo apt-get -y update
sudo apt-get -y upgrade

# Enable universe repo (required for some Qt5 packages on 24.04)
sudo apt-get install -y software-properties-common
sudo add-apt-repository universe
sudo apt-get update

# developer stuff
sudo apt-get install -y build-essential opencl-headers libssl-dev
sudo apt-get install -y libboost-all-dev libcppunit-dev libglew-dev
sudo apt-get install -y qttools5-dev qttools5-dev-tools qtmultimedia5-dev libqt5xmlpatterns5-dev libqt5x11extras5-dev
sudo apt-get install -y ninja-build

# developer IDE's and apps
sudo apt-get install -y cmake-qt-gui qtcreator

# doxygen
sudo apt-get install -y doxygen gnuplot

# coverage testing
sudo apt-get install -y lcov

# source control systems
sudo apt-get install -y subversion cvs git-gui

# v4l
sudo apt-get install -y libv4l-dev

# used by jenkins/CI
sudo apt-get install -y cppcheck tigervnc-standalone-server

# run x11 unit tests on jenkins user
sudo apt-get install -y xvfb

# Dynamic Kernel Module Support (Epiphan dependency)
sudo apt-get install -y dkms

# python libs
sudo apt-get install -y python3-pip python3-lxml

# used by IMFusion
sudo apt-get install -y libcrypto++-dev libnlopt-dev

# utilities
sudo apt-get install -y synaptic aptitude apt-file dia gimp mesa-utils

# vlc
sudo apt-get install -y vlc

# medical image processing app
sudo apt-get install -y itksnap

# webcams
sudo apt-get install -y guvcview cheese qv4l2

# vpn cisco plugin
sudo apt-get install -y network-manager-vpnc-gnome

# connect via ssh (also remember to enable firewall ufw and open port 22)
sudo apt-get install -y openssh-server

# CMake and Qt
sudo apt-get install -y cmake cmake-curses-gui cmake-qt-gui
sudo apt-get install -y qtmultimedia5-dev qttools5-dev
sudo apt-get install -y libxt-dev

# Building robot control Qt SerialPort (Fraxinus)
sudo apt-get install -y libqt5serialport5-dev

# OpenCL (install proprietary NVIDIA drivers separately via "Additional Drivers")
sudo apt install -y nvidia-opencl-dev

# OpenGL / VTK 9: GLVND libraries
sudo apt install -y libglvnd-dev libgl1-mesa-dev

# For enabling vlc integration, and showing H.264 encoded video from VLC
sudo apt install -y vlc-plugin-access-extra gstreamer1.0-libav

# For more convenient use of CustusX build scripts
sudo apt install -y python-is-python3
