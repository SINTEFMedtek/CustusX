cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
    # -----------------------------------------------
    # CppUnit test framework: Create executable testCX
    #
    
    # List of all test units
    QT4_WRAP_CPP( TM_TEST_SOURCE_FILES 
     )
    
    set(TM_TEST_SOURCE_FILES
    cxTest_ToolConfigurationParser.cpp
    ${CX_TEST_SOURCE_FILES}
    )
    
    # A CppUnit executable included for debugging purposes.
    # Not part of ctest
    add_executable(cxTestToolManager cxCppUnitMain.cpp ${TM_TEST_SOURCE_FILES} )
    target_link_libraries(cxTestToolManager toolManager cppunit)
    
    # -----------------------------------------------
    # -- CppUnit - CTest integration. ---------------
    # Use CISST_ADD_TESTS() to convert each CppUnit test to a CTest test. 
    
    # Initialize cisst routines
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/Testing/cisst_code)
    include(cisstMacros)
    
    # Link all CppUnit tests to the cisstTestsMain library
    add_executable(testTM_CppUnit_CTest ${TM_TEST_SOURCE_FILES} )
    target_link_libraries(testTM_CppUnit_CTest cisstTestsMain toolManager cppunit)
    # Generate a CTest list of tests
    CISST_ADD_TESTS(testTM_CppUnit_CTest)
    
    # -- finished CppUnit - CTest integration. ------
    # -----------------------------------------------
endif (BUILD_TESTING)
