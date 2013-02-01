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
		../logic/3DReconstruction
		${CustusX3_SOURCE_DIR}/source/resource
		${CustusX3_SOURCE_DIR}/source/service/patient
		${CustusX3_SOURCE_DIR}/source/service/tracking
		${CustusX3_SOURCE_DIR}/source/resource/settings
		${CustusX3_SOURCE_DIR}/source/resource/utilities
		${CustusX3_SOURCE_DIR}/source/resource/algorithms
		)

	# List of all test units
	QT4_WRAP_CPP( CX_TEST_US_RECONSTRUCTION_SOURCE_FILES
		)
  
	set(CX_TEST_US_RECONSTRUCTION_SOURCE_FILES
		cxTest_UsReconstruction.h
		cxTest_UsReconstruction.cpp
		${CX_TEST_SOURCE_FILES}
		)

	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
	include(cxTestMacros)
	# define cppunit tests and ctest tests from same source:
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestUsReconstruction cxCppUnitMain.cpp "${CX_TEST_US_RECONSTRUCTION_SOURCE_FILES}" cxPluginUSReconstruction)

endif (BUILD_TESTING)
