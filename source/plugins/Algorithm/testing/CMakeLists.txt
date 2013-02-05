###########################################################
##               Template Plugin Tests
##
##               Tests
##
###########################################################

cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
	# -----------------------------------------------
	# CppUnit test framework: Create executable testCX
	#
	include_directories(
		${CPPUNIT_INCLUDE_DIR}
		../logic
		${CustusX3_SOURCE_DIR}/external/ssc/Code
		#${CustusX3_SOURCE_DIR}/source/resource
		#${CustusX3_SOURCE_DIR}/source/service/state
		#${CustusX3_SOURCE_DIR}/source/service/patient
		#${CustusX3_SOURCE_DIR}/source/service/tracking
		#${CustusX3_SOURCE_DIR}/source/resource/settings
		#${CustusX3_SOURCE_DIR}/source/resource/utilities
		#${CustusX3_SOURCE_DIR}/source/resource/algorithms
		#${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction
		#${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic
		#${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic/3DReconstruction
	    ${CustusX3_SOURCE_DIR}/source/logic/
		)

	# List of all test units
	QT4_WRAP_CPP( CX_TEST_ACQUISITION_SOURCE_FILES
		#cxTestAcqController.h
		)

	set(CX_TEST_ACQUISITION_SOURCE_FILES
		${CX_TEST_ACQUISITION_SOURCE_FILES}
		)

    #message(STATUS "CX_USE_TSF: "${CX_USE_TSF})
    if (CX_USE_TSF)
        set(CX_TEST_ACQUISITION_SOURCE_FILES
		    cxTestTubeSegmentationFramework.h
		    cxTestTubeSegmentationFramework.cpp
		    ${CX_TEST_ACQUISITION_SOURCE_FILES}
		)
    endif(CX_USE_TSF)

	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
	include(cxTestMacros)
	# define cppunit tests and ctest tests from same source:
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestTubeSegmentationFramework cxCppUnitMain.cpp "${CX_TEST_ACQUISITION_SOURCE_FILES}" cxPluginAlgorithm)

endif (BUILD_TESTING)
