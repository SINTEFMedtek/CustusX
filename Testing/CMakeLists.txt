cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ automated testing directory
# =========================================================

if (BUILD_TESTING)

    find_package(SSC REQUIRED)
    include (${SSC_USE_FILE} )
    	
    include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )
    include_directories( ${SSC_SOURCE_DIR}/Examples/SliceAndToolRep )
    include_directories( /usr/local/include )
    
    # cppunit
    find_library(CPPUNIT_LIBRARIES cppunit /opt/local/lib)
    
    # test starting the data manager 
    # - ssc::Image
    # - ssc::DataManager 
    add_executable(TestDataManager sscTestDataManager.cpp)
    target_link_libraries( TestDataManager SSC )	
    add_test(TestDataManager TestDataManager)
    
    # test rendering of a 3D volume 
    # - ssc::View
    # - ssc::VolumetricRep
    add_executable(TestVolumetricRep sscTestRenderVolumetricRep.cpp)
    target_link_libraries( TestVolumetricRep SSC )	
    add_test(TestVolumetricRep TestVolumetricRep)
    
    # test adding a mesh to the datamanager.
    # - ssc::Mesh
    add_executable(TestMeshAndDataManager sscTestMeshAndDataManager.cpp)
    target_link_libraries( TestMeshAndDataManager SSC )	
    add_test(TestMeshAndDataManager TestMeshAndDataManager)
    
    # currently disabled - CustusX uses it?
    add_executable(TestMeshDMGeoRepView MACOSX_BUNDLE sscTestMeshDMGeoRepView.cpp)
    target_link_libraries( TestMeshDMGeoRepView SSC )	
    #add_test(TestMeshDMGeoRepView TestMeshDMGeoRepView)
    
    # currently disabled
    # tested class is not in use. 
    #add_executable(TestSliceRep MACOSX_BUNDLE sscTestSliceRep.cpp)
    #target_link_libraries( TestSliceRep SSC )	
    #add_test(TestSliceRep TestSliceRep)
    
    QT4_WRAP_CPP( SSC_TEST_SOURCE_FILES 
        sscViewsWindow.h)
    set(SSC_TEST_SOURCE_FILES
        sscTestSliceComputer.cpp
        sscViewsWindow.cpp 
        sscTestVisualRendering.cpp
        sscTestUtilityClasses.cpp
        ${SSC_TEST_SOURCE_FILES}
        )
        
    add_executable(testSSC 
        sscCPPUnitMain.cpp
        ${SSC_TEST_SOURCE_FILES}
        )
    target_link_libraries(testSSC SSC cppunit)
    set_target_properties(testSSC PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")
    
    # --------------- -------------------------------
    # -- CppUnit - CTest integration. ---------------
    # --   Use CISST_ADD_TESTS() to convert each CppUnit test to a CTest test. 
    
    # Initialize cisst routines
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/Testing/cisst_code)
    include(cisstMacros)
    add_subdirectory(cisst_code)
    
    # link all CppUnit tests to the cisstTestsMain library
    add_executable(testSSC_CppUnit_CTest ${SSC_TEST_SOURCE_FILES} )
    target_link_libraries(testSSC_CppUnit_CTest cisstTestsMain SSC cppunit)
    # Generate a CTest list of tests
    CISST_ADD_TESTS(testSSC_CppUnit_CTest)
    
    # -- finished CppUnit - CTest integration. ------
    # -----------------------------------------------

endif (BUILD_TESTING)
    