#!/bin/bash

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.03.16
#
# Description:
#
#   Experimental!
#   Build part of jenkins CI
#
#####################################################


set -x

cd $WORKSPACE/working/CustusX3/build_Release
cp Testing/`head -n 1 Testing/TAG`/Test.xml ./CTestResults.xml
