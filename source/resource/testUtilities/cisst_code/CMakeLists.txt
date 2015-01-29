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

if(NOT CX_WINDOWS)

    cx_initialize_cppunit()

    project(cisstTests)

    # clipped from cisst/tests/CMakeLists.txt
    # Create the main library
    set(CISST_TESTS_MAIN_SRC
        cisstTestMain.cpp
        cisstTestParameters.cpp
        cisstTestParameters.h)
    add_library(cisstTestsMain ${CISST_TESTS_MAIN_SRC})
    target_link_libraries(cisstTestsMain PRIVATE ${CPPUNIT_LIBRARIES} cxResource)

endif()
