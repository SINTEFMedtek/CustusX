cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Custus unit test folder.
#
# Contains examples of unit tests, using both ctest and cppunit and both
# =========================================================

if (BUILD_TESTING)
	cx_catch_add_master_exe()
endif (BUILD_TESTING)

if (BUILD_TESTING)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource

    ${CustusX3_SOURCE_DIR}/source/service/patient     
    ${CustusX3_SOURCE_DIR}/source/service/tracking     
    ${CustusX3_SOURCE_DIR}/source/service/visualization
    ${CustusX3_SOURCE_DIR}/source/service/video
    ${CustusX3_SOURCE_DIR}/source/service/state

    ${CustusX3_SOURCE_DIR}/source/logic
    ${CustusX3_SOURCE_DIR}/source/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    )    
    # -----------------------------------------------
    # CppUnit test framework: Create executable testCX
    #
    include_directories( ${CPPUNIT_INCLUDE_DIR})
    
    # List of all test units
    QT4_WRAP_CPP( CX_TEST_SOURCE_FILES
        cxTestCustusXController.h 
        )
    set(CX_TEST_SOURCE_FILES
        cxTestSpeed.cpp
        cxTestSpeed.h
        cxTestCustusXController.cpp
        cxTestCustusXController.h
        ${CX_TEST_SOURCE_FILES}
        )
        
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
	include(cxTestMacros)
	# define cppunit tests and ctest tests from same source:
	SSC_ADD_CTEST_CPPUNIT_TESTS(testCX cxCppUnitMain.cpp "${CX_TEST_SOURCE_FILES}" cxGui)
endif (BUILD_TESTING)
