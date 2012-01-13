#!/bin/bash

#####################################################
# Runc script for CustusX
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2012.01.12
#
# Description:
#
#	Run the CustusX application.
#   
#   This script enables libraries to be included from a
#   path relative to the application. 
#   
#   Ref http://www.cmake.org/Wiki/CMake_RPATH_handling
#   for a description of the problem. 
#
#####################################################

export LD_LIBRARY_PATH=bin:.:$LD_LIBRARY_PATH
./bin/CustusX
