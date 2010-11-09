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
        
    # A CppUnit executable included for debugging purposes.
    # Not part of ctest
    add_executable(testCX cxCppUnitMain.cpp ${CX_TEST_SOURCE_FILES} )
    target_link_libraries(testCX SSC gui ${CPPUNIT_LIBRARIES})
    
    # -----------------------------------------------
    # -- CppUnit - CTest integration. ---------------
    # 
    # Use CISST_ADD_TESTS() to convert each CppUnit test to a CTest test. 
    
    # Initialize cisst routines
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/Testing/cisst_code)
    include(cisstMacros)
    
    # Link all CppUnit tests to the cisstTestsMain library
    add_executable(testCX_CppUnit_CTest ${CX_TEST_SOURCE_FILES} )
    target_link_libraries(testCX_CppUnit_CTest cisstTestsMain SSC gui ${CPPUNIT_LIBRARIES})
    # Generate a CTest list of tests
    CISST_ADD_TESTS(testCX_CppUnit_CTest)
    
    # -- finished CppUnit - CTest integration. ------
    # -----------------------------------------------
endif (BUILD_TESTING)
