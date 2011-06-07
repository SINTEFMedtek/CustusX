cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
    # -----------------------------------------------
    # CppUnit test framework: Create executable testCX
    #
    include_directories( ${CPPUNIT_INCLUDE_DIR})
    
    # List of all test units
    QT4_WRAP_CPP( CX_TEST_SOURCE_FILES
        )
    set(CX_TEST_SOURCE_FILES
        cxTestAlgorithms.h
        cxTestAlgorithms.cpp
        ${CX_TEST_SOURCE_FILES}
        )
        
    # A CppUnit executable included for debugging purposes.
    # Not part of ctest
    add_executable(testAlgorithms cxCppUnitMain.cpp ${CX_TEST_SOURCE_FILES} )
    target_link_libraries(testAlgorithms SSC cxAlgorithms ${CPPUNIT_LIBRARIES})
    
    # -----------------------------------------------
    # -- CppUnit - CTest integration. ---------------
    # 
    # Use CISST_ADD_TESTS() to convert each CppUnit test to a CTest test. 
    
    # Initialize cisst routines
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/Testing/cisst_code)
    include(cisstMacros)
    
    # Link all CppUnit tests to the cisstTestsMain library
    add_executable(testAlgorithms_CppUnit_CTest ${CX_TEST_SOURCE_FILES} )
    target_link_libraries(testAlgorithms_CppUnit_CTest cisstTestsMain SSC cxAlgorithms ${CPPUNIT_LIBRARIES})
    # Generate a CTest list of tests
    CISST_ADD_TESTS(testAlgorithms_CppUnit_CTest)
    
    # -- finished CppUnit - CTest integration. ------
    # -----------------------------------------------
endif (BUILD_TESTING)
