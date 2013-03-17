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
CX_RELEASE_DIR=$WORKSPACE/working/CustusX3/build_Release

# ==========================================================
# Download, configure and build the project group.
# Python script will return success even if some parts failed.
./working/CustusX3/CustusX3/install/Shared/script/cxInstaller.py --full --all -t Release -j4 -s --isb_password=sintefsvn -u custusx --external_dir=$WORKSPACE/external --working_dir=$WORKSPACE/working --cmake_args="-DBUILD_DOCUMENTATION:BOOL=ON"
# use during development: update only the CustusX part
./working/CustusX3/CustusX3/install/Shared/script/cxInstaller.py --full -t Release -j4 -s --isb_password=sintefsvn -u custusx --external_dir=$WORKSPACE/external --working_dir=$WORKSPACE/working --cmake_args="-DBUILD_DOCUMENTATION:BOOL=ON" CustusX3

#make -j4
#ctest -R Thunder -V
#ctest -D ExperimentalTest --no-compress-output -R Tool
#$WORKSPACE/working/CustusX3/CustusX3/testing/jenkins/build_step.sh

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
echo "checking pwd"
echo `pwd`
# execute tests with xml output
ctest -D ExperimentalTest --no-compress-output -R Tool
# copy xml to fixed position ./CTestResults.xml
cp Testing/`head -n 1 Testing/TAG`/Test.xml ./CTestResults.xml

# ==========================================================
# copy/publish doxygen to medtek server (link from wiki):
REMOTE_DOXY_PATH="/Volumes/medtek_HD/Library/Server/Web/Data/Sites/Default/custusx_doxygen"
scp -r $CX_RELEASE_DIR/doc/doxygen/html/* medtek.sintef.no:$REMOTE_DOXY_PATH
