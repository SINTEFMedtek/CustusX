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
		${CustusX3_SOURCE_DIR}/source/resource
		${CustusX3_SOURCE_DIR}/source/resource/settings
		${CustusX3_SOURCE_DIR}/source/resource/utilities
		${CustusX3_SOURCE_DIR}/source/resource/algorithms
		${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
        ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction
		)

	# List of all test units
	QT4_WRAP_CPP( CX_TEST_ACQUISITION_SOURCE_FILES
		)

	set(CX_TEST_ACQUISITION_SOURCE_FILES
		cxTestIGTLinkUtilities.h
		cxTestIGTLinkUtilities.cpp
		${CX_TEST_ACQUISITION_SOURCE_FILES}
		)

	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
	include(cxTestMacros)
	# define cppunit tests and ctest tests from same source:
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestIGTLinkUtilities cxCppUnitMain.cpp "${CX_TEST_ACQUISITION_SOURCE_FILES}" cxOpenIGTLinkUtilities)

endif (BUILD_TESTING)
