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

add_executable(TestRender sscTestRender.cpp)
target_link_libraries( TestRender SSC )	
add_test(TestRender TestRender)

add_executable(TestDataManager sscTestDataManager.cpp)
target_link_libraries( TestDataManager SSC )	
add_test(TestDataManager TestDataManager)

add_executable(TestMeshAndDataManager sscTestMeshAndDataManager.cpp)
target_link_libraries( TestMeshAndDataManager SSC )	
add_test(TestMeshAndDataManager TestMeshAndDataManager)

add_executable(TestMeshDMGeoRepView MACOSX_BUNDLE sscTestMeshDMGeoRepView.cpp)
target_link_libraries( TestMeshDMGeoRepView SSC )	
add_test(TestMeshDMGeoRepView TestMeshDMGeoRepView)

# sscTestSliceRep
add_executable(TestSliceRep MACOSX_BUNDLE sscTestSliceRep.cpp)
target_link_libraries( TestSliceRep SSC )	
add_test(TestSliceRep TestSliceRep)

# sscTestSliceAndToolRep
SET( SSCSliceAndToolRep_QT_MOC_HEADER_FILES
    sscTestSliceAndToolRep.h
   )
set( MOC_HEADER_FILES )
QT4_WRAP_CPP( MOC_HEADER_FILES ${SSCSliceAndToolRep_QT_MOC_HEADER_FILES} )

add_executable(TestSliceAndToolRep MACOSX_BUNDLE ${MOC_HEADER_FILES} sscTestSliceAndToolRep.cpp)
target_link_libraries( TestSliceAndToolRep SSC )	
add_test(TestSliceRep TestSliceRep)

add_executable(TestRenderSpeed MACOSX_BUNDLE ${MOC_HEADER_FILES} sscTestRenderSpeed.cpp)
target_link_libraries( TestRenderSpeed SSC )	


#sscTestSlicePlaneRep
SET( sscTestSlicePlaneRep_QT_MOC_HEADER_FILES
    sscTestSlicePlaneRep.h
   )
set( MOC_HEADER_FILES )
QT4_WRAP_CPP( MOC_HEADER_FILES ${sscTestSlicePlaneRep_QT_MOC_HEADER_FILES} )

add_executable(TestPlaneRep MACOSX_BUNDLE ${MOC_HEADER_FILES} sscTestSlicePlaneRep.cpp)
target_link_libraries( TestPlaneRep SSC )	
add_test(TestPlaneRep TestPlaneRep)


#sscTestSliceAndOverlayImage
SET( sscTestSliceAndOverlayImage_QT_MOC_HEADER_FILES
    sscTestSliceAndOverlayImage.h    
   )
set( MOC_HEADER_FILES )
QT4_WRAP_CPP( MOC_HEADER_FILES ${sscTestSliceAndOverlayImage_QT_MOC_HEADER_FILES} )
add_executable(TestSliceAndOverlayImage MACOSX_BUNDLE ${MOC_HEADER_FILES} sscTestSliceAndOverlayImage.cpp)
target_link_libraries( TestSliceAndOverlayImage SSC )	
add_test(TestSliceAndOverlayImage TestSliceAndOverlayImage)

#sscTestCPPunit tes
add_executable(sscTestCPPUnit
    sscCPPUnitMain.cpp
    sscTestSliceComputer.cpp )
target_link_libraries( sscTestCPPUnit SSC cppunit)

