cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
include_directories( ${CPPUNIT_INCLUDE_DIR})
    
    # List of all test units
    QT4_WRAP_CPP( TM_TEST_SOURCE_FILES 
     )
    
    set(TM_TEST_SOURCE_FILES
        cxTest_ToolConfigurationParser.h
        cxTest_ToolConfigurationParser.cpp
        cxTest_Tracker.h
        cxTest_Tracker.cpp
        cxTest_Tool.h
        cxTest_Tool.cpp
        ${CX_TEST_SOURCE_FILES}
    )
    
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
	include(cxTestMacros)
	# define cppunit tests and ctest tests from same source:
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestToolManager cxCppUnitMain.cpp "${TM_TEST_SOURCE_FILES}" cxTrackingService)
endif (BUILD_TESTING)
