#
# $Id: CMakeLists.txt,v 1.27 2007/04/26 20:12:05 anton Exp $
#

# (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# backward compatibility
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
PROJECT(cisstTests)

# clipped from cisst/tests/CMakeLists.txt
# Create the main library
SET(CISST_TESTS_MAIN_SRC
    cisstTestMain.cpp
    cisstTestParameters.cpp
    cisstTestParameters.h)
ADD_LIBRARY(cisstTestsMain ${CISST_TESTS_MAIN_SRC})
target_link_libraries(cisstTestsMain ${CPPUNIT_LIBRARIES} ${QT_LIBRARIES} ${SSC_GCOV_LIBRARY} SSC)

