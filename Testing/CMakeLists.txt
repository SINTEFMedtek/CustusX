cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ automated testing directory
# =========================================================

find_package(SSC REQUIRED)
include (${SSC_USE_FILE} )
	
include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )
include_directories( ${SSC_SOURCE_DIR}/Examples/SliceAndToolRep )

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

# test ACS+3D with one volume and a moving tool
# - ssc::ToolManager
# - ssc::Tool
# - ssc::SliceProxy
# - ssc::SlicerRepSW
#
#SET( SSCSliceAndToolRep_QT_MOC_HEADER_FILES sscTestSliceAndToolRep.h)
#set( MOC_HEADER_FILES )    
#QT4_WRAP_CPP( MOC_HEADER_FILES ${SSCSliceAndToolRep_QT_MOC_HEADER_FILES} )
#add_executable(TestVisualRendering MACOSX_BUNDLE ${MOC_HEADER_FILES}
#    sscTestSliceAndToolRep.cpp 
#    sscTestVisualRendering.cpp)
#target_link_libraries( TestVisualRendering SSC )	
#add_test(TestVisualRendering TestVisualRendering)


# test slicing in 3D
# - ssc::SlicePlaneRep
#
# incomplete - 3 of 4 views are empty
# 
#SET( sscTestSlicePlaneRep_QT_MOC_HEADER_FILES sscTestSlicePlaneRep.h)
#set( MOC_HEADER_FILES )
#QT4_WRAP_CPP( MOC_HEADER_FILES ${sscTestSlicePlaneRep_QT_MOC_HEADER_FILES} )
#add_executable(TestPlaneRep MACOSX_BUNDLE ${MOC_HEADER_FILES} sscTestSlicePlaneRep.cpp)
#target_link_libraries( TestPlaneRep SSC )	
#add_test(TestPlaneRep TestPlaneRep)

## TODO: 
# create a single slice test 
# create a ACS+3D test 
# create a AnyDual+3D test
# create a 3 volumes stress test
# create a progressive volumetric rep test?? or maybe dont spend time on this...?
#
# move code to a test app in Examples
#
# add a "verify good images" button as started on in testRenderSpeed. use automatic/semiautomatic mode.
#
# remove the tests in test folder that are not applicable as autotests.
#
#

# TODO: change name of this to a better name.
#       keep a stress test in the auto dir,
#       move something like this to the examples folder. 
#
# test 3 simultaneous volumes in ACS mode
# test 3 simultaneous volumes in Any/Dual mode
# - ssc::SliceProxy
# - ssc::SlicerRepSW
#
#add_executable(TestRenderSpeed MACOSX_BUNDLE ${MOC_HEADER_FILES} sscTestRenderSpeed.cpp)
#target_link_libraries( TestRenderSpeed SSC )	
#add_test(TestRenderSpeed TestRenderSpeed)

QT4_WRAP_CPP( SSC_TEST_SOURCE_FILES 
    sscViewsWindow.h)
set(SSC_TEST_SOURCE_FILES
    sscTestSliceComputer.cpp
	sscTestSpaceOrganizer.cpp
    sscViewsWindow.cpp 
    sscTestVisualRendering.cpp
    sscTestUtilityClasses.cpp
    ${SSC_TEST_SOURCE_FILES}
    )
    
#add_library(sscTestLib ${SSC_TEST_SOURCE_FILES})
#target_link_libraries(sscTestLib SSC cppunit)
#set_target_properties(sscTestLib PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")

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
add_executable(myFirst_CPPUNIT_CTEST_TEST ${SSC_TEST_SOURCE_FILES} )
target_link_libraries(myFirst_CPPUNIT_CTEST_TEST cisstTestsMain SSC cppunit)
# Generate a CTest list of tests
CISST_ADD_TESTS(myFirst_CPPUNIT_CTEST_TEST)

# -- finished CppUnit - CTest integration. ------
# -----------------------------------------------
