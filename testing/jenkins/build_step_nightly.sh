#!/bin/bash

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.03.16
#
# Description:
#
#   Experimental!
#   Build part of jenkins CI
#
#       Download, build, and test CustusX
#       Publish unit tests
#       Publish doxygen doc to medtek.sintef.no
#
#
#
#####################################################

set -x
WORKSPACE=$1
ISB_PASSWORD=$2
CX_RELEASE_DIR=$WORKSPACE/working/CustusX3/build_Release

# assume workspace is cleaned before start.

# ==========================================================
# Download, configure and build the project group.
# Python script will return success even if some parts failed.
./working/CustusX3/CustusX3/install/Shared/script/cxInstaller.py --full --all -t Release -j4 -s --isb_password=$ISB_PASSWORD -u custusx --external_dir=$WORKSPACE/external --working_dir=$WORKSPACE/working --cmake_args="-DBUILD_DOCUMENTATION:BOOL=ON"

# ==========================================================
# make the CustusX project in order to provoke a build failure.
cd $CX_RELEASE_DIR
make -j4
if [ $? == 0 ]
then
    echo "CustusX build success"
#    exit 0
else
    echo "CustusX build failure, terminating"
    exit 1
fi

# ==========================================================
# Run all tests and write them in xml format to ./CTestResults.xml
cd $CX_RELEASE_DIR
rm -rf $CX_RELEASE_DIR/Testing/[0-9]*
rm -rf $CX_RELEASE_DIR/CTestResults.xml
ctest -D ExperimentalTest --no-compress-output
cp Testing/`head -n 1 Testing/TAG`/Test.xml ./CTestResults.xml

# ==========================================================
# copy/publish doxygen to medtek server (link from wiki):
REMOTE_DOXY_PATH="/Volumes/medtek_HD/Library/Server/Web/Data/Sites/Default/custusx_doxygen"
scp -r $CX_RELEASE_DIR/doc/doxygen/html/* medtek.sintef.no:$REMOTE_DOXY_PATH
