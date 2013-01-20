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

echo "Generating coverage from cwd..."

echo ""
set -x

# remove previous coverage counts - not working
#lcov --zerocounters -directory .

#kjør alle tester

# convert coverage data from app runs
lcov --capture --directory . --output-file cx_coverage.gcov
# remove system and external libraries
lcov --remove cx_coverage.gcov 'eigen3/Eigen/*' '/opt/*' 'external_code/*' '/Library/*' '/usr/*' --output-file cx_coverage.gcov
# generate html
genhtml cx_coverage.gcov -output-directory coverage_info
# open in web browser
open coverage_info/index.html



set +x

echo "===== lcov has been run, results opened in web browser ====="
echo ""
echo ""
