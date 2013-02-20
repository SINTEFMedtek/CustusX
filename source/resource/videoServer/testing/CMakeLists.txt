cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
	# -----------------------------------------------
	# CppUnit test framework: Create executable testCX
	#
	include_directories(
                ${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
                ${CustusX3_SOURCE_DIR}/source/resource/videoServer
                ${CPPUNIT_INCLUDE_DIR}
		)

	# List of all test units
	QT4_WRAP_CPP( CX_TEST_SOURCE_FILES
		cxTestGEInterfaceController.h
		)

	set(CX_TEST_SOURCE_FILES
		cxTestGEInterface.h
		cxTestGEInterface.cpp
		cxTestGEInterfaceController.h
		cxTestGEInterfaceController.cpp
		${CX_TEST_SOURCE_FILES}
		)

	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
	include(cxTestMacros)
	# define cppunit tests and ctest tests from same source:
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestGEInterface 
	    cxCppUnitMain.cpp 
	    "${CX_TEST_SOURCE_FILES}" 
	    cxGrabber)

endif (BUILD_TESTING)
