#!/bin/bash

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2011.08.11
#
# Description:	
#
#    Install git using yum.
#    Configure the git installation. 
#
#
#####################################################

#USER=`whoami`

if [[ "$1" == "" ]] ; then
    echo "Usage: cx_run_lcov <path_to_build_root>"
    exit
fi

echo "Generating coverage from $1"

echo ""
set -x

# remove previous coverage counts - not working
lcov -capture -initial --directory $1 --output-file cx_coverage_base.gcov
#lcov --zerocounters -directory .

#kjør alle tester
#ctest

# convert coverage data from app runs
lcov --capture --directory $1 --output-file cx_coverage_test.gcov
lcov -add-tracefile cx_coverage_base.gcov -a cx_coverage_test.gcov -o cx_coverage_total.gcov
# remove system and external libraries
lcov --remove cx_coverage.gcov 'eigen3/Eigen/*' '/opt/*' 'external_code/*' '/Library/*' '/usr/*' '/moc*.cxx' --output-file cx_coverage.gcov
# generate html
genhtml cx_coverage.gcov -output-directory $1/coverage_info
# open in web browser
#open coverage_info/index.html # mac
firefox $1/coverage_info/index.html



set +x

echo "===== lcov has been run, results opened in web browser ====="
echo ""
echo ""
