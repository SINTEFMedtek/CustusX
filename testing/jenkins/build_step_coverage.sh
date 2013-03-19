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
#./working/CustusX3/CustusX3/install/Shared/script/cxInstaller.py --full -t Debug -j4 -s --isb_password=sintefsvn -u custusx --external_dir=$WORKSPACE/external --working_dir=$WORKSPACE/working --cmake_args="-DBUILD_DOCUMENTATION:BOOL=OFF" --coverage CustusX3

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

GCOV_TEMP_DIR=$WORKSPACE/gcov
GCOV_RESULT_DIR=$WORKSPACE/gcov/coverage_info

# remove old test data before start
#cd $CX_DEBUG_DIR
rm -rf $CX_DEBUG_DIR/Testing/[0-9]*
rm -rf $CX_DEBUG_DIR/CTestResults.xml

# ==========================================================
# run lcov and generate html. Coverage info will now be in CX_DEBUG_DIR/coverage_info

# ==========================================================
# Initialize lcov by resetting all existing counters
# and initializing/generating counters for all files.
rm -rf $GCOV_TEMP_DIR
rm -rf $GCOV_RESULT_DIR
mkdir -p $GCOV_TEMP_DIR
mkdir -p $GCOV_RESULT_DIR
lcov --zerocounters -directory $CX_DEBUG_DIR
lcov --capture --initial --directory $CX_DEBUG_DIR --output-file $GCOV_TEMP_DIR/cx_base.gcov

# ==========================================================
# run tests
cd $CX_DEBUG_DIR
ctest -D ExperimentalTest --no-compress-output
#cd $WORKSPACE
#./working/CustusX3/CustusX3/testing/coverage/cxCoverage.py --initialize --run_tests --ctest_args="-D ExperimentalTest --no-compress-output" --post_test $CX_DEBUG_DIR

# ==========================================================
# Given that lcov is initialized and ctest is run,
# Generate html output from the gcov data.
lcov --capture --directory $CX_DEBUG_DIR --output-file $GCOV_TEMP_DIR/cx_test.gcov
lcov --add-tracefile $GCOV_TEMP_DIR/cx_base.gcov --add-tracefile $GCOV_TEMP_DIR/cx_test.gcov --output-file $GCOV_TEMP_DIR/cx_total.gcov
lcov --remove $GCOV_TEMP_DIR/cx_total.gcov "/eigen3/Eigen/*" "/opt/*" "/external/*" "/external_code/*" "/Library/*" "/usr/*" "/moc*.cxx" "/CustusX3/build_*" "/Examples/*" --output-file $GCOV_TEMP_DIR/cx_clean.gcov
genhtml $GCOV_TEMP_DIR/cx_clean.gcov --output-directory $GCOV_RESULT_DIR

# ==========================================================
# RunWrite tests in xml format to ./CTestResults.xml
cp $CX_DEBUG_DIR/Testing/`head -n 1 $CX_DEBUG_DIR/Testing/TAG`/Test.xml $CX_DEBUG_DIR/CTestResults.xml

# ==========================================================
# run cppcheck
CX_SOURCE_DIR=$WORKSPACE/working/CustusX3/CustusX3
cppcheck --enable=all --xml-version=2 -i$CX_SOURCE_DIR/externals/ssc/Code/3rdParty/ $CX_SOURCE_DIR/source $CX_SOURCE_DIR/externals/ssc/Code/ 2> $WORKSPACE/cppcheck-result.xml

# ==========================================================
# run line counter
sloccount --duplicates --wide --details $CX_SOURCE_DIR >$WORKSPACE/sloccount_raw.sc
$CX_SOURCE_DIR/testing/jenkins/clean_sloccount.py --remove="3rdParty/ config/ install/ /data/" $WORKSPACE/sloccount_raw.sc $WORKSPACE/sloccount.sc

# post-op requirements:
#   - publish coverage data 
#   - publish ctest data using xUnit
#   - publish cppcheck and sloccount


