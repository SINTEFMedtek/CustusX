#!/bin/bash

#####################################################
# Run script for CustusX
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.04.09
#
# Description:
#
#	Run a bundled elastix executable (http://elastix.isi.uu.nl)
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
ELASTIX_VERSION='v4.6'

function run_linux 
{
	PLATFORM_DIR='elastix_linux64_'$ELASTIX_VERSION
	# enable linking to bin folder where all the dynamic libs are
	export LD_LIBRARY_PATH=$DIR/$PLATFORM_DIR/lib:$LD_LIBRARY_PATH
	$DIR/$PLATFORM_DIR/bin/elastix "$@"
}

function run_mac 
{
	PLATFORM_DIR='elastix_macosx64_'$ELASTIX_VERSION
	# enable linking to bin folder where all the dynamic libs are
	export DYLD_LIBRARY_PATH=$DIR/$PLATFORM_DIR/lib:$LD_LIBRARY_PATH
	$DIR/$PLATFORM_DIR/bin/elastix "$@"
}

# http://stackoverflow.com/questions/394230/detect-the-os-from-a-bash-script
PLATFORM=`uname -s`
# http://stackoverflow.com/questions/3190818/pass-all-arguments-from-bash-script-to-another-command

if [[ "$PLATFORM" == 'Linux' ]]; then
	run_linux "$@"
elif [[ "$PLATFORM" == 'Darwin' ]]; then
	run_mac "$@"
else
	echo 'Error: No elastix binary found for platform: ' $PLATFORM
fi



