#!/bin/bash

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.03.16
#
# Description:
#
#   Continous integration build: run every time the repo changes.
#   Experimental!
#   Build part of jenkins CI
#
#       Download, build, and test CustusX
#       Publish unit tests
#
#
#####################################################

set -x
WORKSPACE=$1
ISB_PASSWORD=$2
CX_SOURCE_DIR=$WORKSPACE/working/CustusX3/CustusX3
CX_RELEASE_DIR=$WORKSPACE/working/CustusX3/build_Release
CX_CATCH_DIR=$CX_RELEASE_DIR/source/testing
CX_DATA_TEMP_DIR=CX_SOURCE_DIR/Data/temp

# ==========================================================
# Download, configure and build the project group.
# Python script will return success even if some parts failed.
./working/CustusX3/CustusX3/install/Shared/script/cxInstaller.py --full --all -t Release -j4 -s --isb_password=$ISB_PASSWORD -u custusx --external_dir=$WORKSPACE/external --working_dir=$WORKSPACE/working --cmake_args="-DBUILD_DOCUMENTATION:BOOL=OFF"

# clear local modifications in the data folder - the tests might cause these changes
cd $CX_SOURCE_DIR/data
git fetch --all
git reset --hard

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
# Clean up TSF data from last run (make more general?)
rm -rf CX_DATA_TEMP_DIR/TubeSegmentationFramework/*

# ==========================================================
# Run all tests and write them in xml format to ./CTestResults.xml
cd $CX_RELEASE_DIR
rm -rf Testing/[0-9]*
rm -rf $CX_RELEASE_DIR/CTestResults.xml
ctest -D ExperimentalTest --no-compress-output
cp Testing/`head -n 1 Testing/TAG`/Test.xml ./CTestResults.xml

# ==========================================================
# Run all Catch tests and write them in xml format to ./CatchTestResults.xml
cd $CX_CATCH_DIR
rm -rf CX_RELEASE_DIR/CatchTestResults.xml

PLATFORM=`uname -s`
if [[ "$PLATFORM" == 'Darwin' ]]; then
./Catch.app/Contents/MacOS/Catch -r junit -o CatchTestResults.xml
else
./Catch -r junit -o CatchTestResults.xml
fi

cp CatchTestResults.xml $CX_RELEASE_DIR/CatchTestResults.xml

# ==========================================================
# package the build
cd $CX_RELEASE_DIR
make package
if [ $? == 0 ]
then
    echo "CustusX make package success"
else
    echo "CustusX make package failure, terminating"
    exit 1
fi

