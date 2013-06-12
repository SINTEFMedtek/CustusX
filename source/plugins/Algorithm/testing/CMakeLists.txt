# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
#
# CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
# code and binaries can only be used by SMT and those with explicit permission
# from SMT. CustusX shall not be distributed to anyone else.
#
# CustusX is a research tool. It is NOT intended for use or certified for use
# in a normal clinical setting. SMT does not take responsibility for its use
# in any way.
#
# See CustusX_License.txt for more information.

###########################################################
#               cxPluginAlgorithm Tests
###########################################################

cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(BUILD_TESTING)
	# Catch unit test framework stuff:

	QT4_WRAP_CPP(CXTEST_PLUGINALGORITHM_SOURCES
		)
	set(CXTEST_PLUGINALGORITHM_SOURCES ${CXTEST_PLUGINALGORITHM_SOURCES}
        cxCatchAlgorithm.cpp)

	cx_catch_add_lib_and_exe(cxPluginAlgorithm "${CXTEST_PLUGINALGORITHM_SOURCES}")

endif(BUILD_TESTING)


if (BUILD_TESTING)
	# -----------------------------------------------
	# CppUnit test framework: Create executable testCX
	#
	include_directories(
		${CPPUNIT_INCLUDE_DIR}
		../logic
		${CustusX3_SOURCE_DIR}/external/ssc/Code
	    ${CustusX3_SOURCE_DIR}/source/logic/
		)

	# List of all test units
	QT4_WRAP_CPP( CX_TEST_ACQUISITION_SOURCE_FILES
		)

	set(CX_TEST_ACQUISITION_SOURCE_FILES
		${CX_TEST_ACQUISITION_SOURCE_FILES}
		)

    if (CX_USE_TSF)
        set(CX_TEST_ACQUISITION_SOURCE_FILES
		    cxTestTubeSegmentationFramework.h
		    cxTestTubeSegmentationFramework.cpp
		    cxTestTSFPresets.h
		    cxTestTSFPresets.cpp
		    ${CX_TEST_ACQUISITION_SOURCE_FILES}
		)

		set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
		include(cxTestMacros)
		# define cppunit tests and ctest tests from same source:
		SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestTubeSegmentationFramework cxCppUnitMain.cpp "${CX_TEST_ACQUISITION_SOURCE_FILES}" cxPluginAlgorithm)
    endif(CX_USE_TSF)

endif (BUILD_TESTING)
