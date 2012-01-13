#!/bin/bash

#####################################################
# Run script for CustusX
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

# http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
DIR="$( cd "$( dirname "$0" )" && pwd )" 

# enable linking to bin folder where all the dynamic libs are
export LD_LIBRARY_PATH=$DIR/bin:$DIR:$LD_LIBRARY_PATH

# run CustusX
$DIR/bin/CustusX
