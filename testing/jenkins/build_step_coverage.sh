#!/bin/bash

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.03.16
#
# Description:
#
#    Generate lcov coverage reports.
#    Assumes all data has been downloaded by another script.
#
#
#####################################################

set -x
WORKSPACE=$1
CX_DEBUG_DIR=$WORKSPACE/working/CustusX3/build_Debug

# ==========================================================
# configure and build all libs with debug info
./working/CustusX3/CustusX3/install/Shared/script/cxInstaller.py --full --all -t Debug -j4 -s --isb_password=sintefsvn -u custusx --external_dir=$WORKSPACE/external --working_dir=$WORKSPACE/working --cmake_args="-DBUILD_DOCUMENTATION:BOOL=OFF" --coverage

# ==========================================================
# make the CustusX project in order to provoke a build failure.
cd $CX_DEBUG_DIR
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
# run lcov and generate html. Coverage info will now be in CX_DEBUG_DIR/
cd $WORKSPACE
./working/CustusX3/CustusX3/testing/coverage/cxCoverage.py --initialize --run_tests --post_test $CX_DEBUG_DIR
