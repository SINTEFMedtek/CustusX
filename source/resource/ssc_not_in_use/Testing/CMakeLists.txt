cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ automated testing directory
# =========================================================

if (BUILD_TESTING)
        
    include_directories( ${CX_SSC_INCLUDE_DIRECTORIES} )
    include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )
    include_directories( ${SSC_SOURCE_DIR}/Examples/SliceAndToolRep )
    include_directories( /usr/local/include )
    include_directories( /opt/local/include )
    
    # test starting the data manager 
    # - Image
    # - DataManager 
    add_executable(TestDataManager sscTestDataManager.cpp)
    target_link_libraries( TestDataManager SSC )	
    set_target_properties(TestDataManager PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")
    add_test(TestDataManager TestDataManager)
	#SETUP_TARGET_FOR_COVERAGE(CoverageTestDataManager TestDataManager CoverageTestDataManager.xml)
    
	# test rendering of a 3D volume
	# - View
	# - ProgressiveLODVolumetricRep
    # only working on Linux:
    if(NOT APPLE)
		add_executable(TestVolumetricRep sscTestRenderVolumetricRep.cpp)
		target_link_libraries( TestVolumetricRep SSC )
		set_target_properties(TestVolumetricRep PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")
		add_test(TestVolumetricRep TestVolumetricRep)
    endif()
    
    # test adding a mesh to the datamanager.
    # - Mesh
    add_executable(TestMeshAndDataManager sscTestMeshAndDataManager.cpp)
    target_link_libraries( TestMeshAndDataManager SSC )	
    set_target_properties(TestMeshAndDataManager PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")
    add_test(TestMeshAndDataManager TestMeshAndDataManager)
    
    # currently disabled - CustusX uses it?
    add_executable(TestMeshDMGeoRepView MACOSX_BUNDLE sscTestMeshDMGeoRepView.cpp)
    set_target_properties(TestMeshDMGeoRepView PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")
    target_link_libraries( TestMeshDMGeoRepView SSC )	
    #add_test(TestMeshDMGeoRepView TestMeshDMGeoRepView)
    
    # test fiber bundle rep
    # - FiberBundle
    # - FiberBundleRep
    add_executable(TestFiberBundleRep sscTestFiberBundleRep.cpp)
    target_link_libraries( TestFiberBundleRep SSC )	
    set_target_properties(TestFiberBundleRep PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")
    add_test(TestFiberBundleRep TestFiberBundleRep)

    # currently disabled
    # tested class is not in use. 
    #add_executable(TestSliceRep MACOSX_BUNDLE sscTestSliceRep.cpp)
    #target_link_libraries( TestSliceRep SSC )	
    #add_test(TestSliceRep TestSliceRep)
    
    QT4_WRAP_CPP( SSC_TEST_SOURCE_FILES 
        sscViewsWindow.h
        sscContainerWindow.h)
    set(SSC_TEST_SOURCE_FILES
        sscTestSliceComputer.cpp
        sscViewsWindow.cpp 
        sscContainerWindow.cpp
        sscTestVisualRendering.cpp
        sscTestViewContainer.cpp
        sscTestUtilityClasses.cpp
        sscTestTransform3D.cpp
        sscTestBoundingBox3D.cpp
        sscTestGPURayCaster.cpp
        sscTestVideoGraphics.h
        sscTestVideoGraphics.cpp
        ${SSC_TEST_SOURCE_FILES}
        )
        

	include(cxTestMacros)
	add_subdirectory(cisst_code)

	# define cppunit tests and ctest tests from same source:
	SSC_ADD_CTEST_CPPUNIT_TESTS(testSSC sscCPPUnitMain.cpp "${SSC_TEST_SOURCE_FILES}" SSC)

endif (BUILD_TESTING)
    
