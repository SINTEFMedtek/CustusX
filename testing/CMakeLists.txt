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

    find_package(SSC REQUIRED)
    include (${SSC_USE_FILE} )
    	
    include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )
    include_directories( ${SSC_SOURCE_DIR}/Examples/SliceAndToolRep )
    include_directories( /usr/local/include )
    
    if (APPLE AND EXISTS /opt/local/lib) 
        # location of cppunit library if installed using macport
        link_directories( /opt/local/lib )
    endif (APPLE AND EXISTS /opt/local/lib)    
    
    # test starting the data manager 
    # - ssc::Image
    # - ssc::DataManager 
    add_executable(cxTest_Example_Simple cxTest_Example_Simple.cpp)
    target_link_libraries( cxTest_Example_Simple SSC )	
    add_test(cxTest_Example_Simple cxTest_Example_Simple)
    
    
    # test rendering of a 3D volume 
    # - ssc::View
    # - ssc::VolumetricRep
    add_executable(cxTest_Example_RenderVolumetricRep cxTest_Example_RenderVolumetricRep.cpp)
    target_link_libraries( cxTest_Example_RenderVolumetricRep SSC )	
    add_test(cxTest_Example_RenderVolumetricRep cxTest_Example_RenderVolumetricRep)
    
    
    # -----------------------------------------------
    # CppUnit test framework: Create executable testCX
    #
    
    # List of all test units
    QT4_WRAP_CPP( CX_TEST_SOURCE_FILES 
        )
    set(CX_TEST_SOURCE_FILES
        cxTest_Example_CppUnit.cpp
        ${CX_TEST_SOURCE_FILES}
        )
        
    # A CppUnit executable included for debugging purposes.
    # Not part of ctest
    add_executable(testCX cxCppUnitMain.cpp ${CX_TEST_SOURCE_FILES} )
    target_link_libraries(testCX SSC cppunit)
    
    # -----------------------------------------------
    # -- CppUnit - CTest integration. ---------------
    # 
    # Use CISST_ADD_TESTS() to convert each CppUnit test to a CTest test. 
    
    # Initialize cisst routines
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/Testing/cisst_code)
    include(cisstMacros)
    
    # Link all CppUnit tests to the cisstTestsMain library
    add_executable(testCX_CppUnit_CTest ${CX_TEST_SOURCE_FILES} )
    target_link_libraries(testCX_CppUnit_CTest cisstTestsMain SSC cppunit)
    # Generate a CTest list of tests
    CISST_ADD_TESTS(testCX_CppUnit_CTest)
    
    # -- finished CppUnit - CTest integration. ------
    # -----------------------------------------------
endif (BUILD_TESTING)
