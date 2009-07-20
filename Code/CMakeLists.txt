cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ Library 
# =========================================================

# =========================================================
# Define variables for paths and include files.
# =========================================================

# Must be the same as the project name. Replaces CMAKE_PROJECT_NAME,
# which I cannot get to work properly
#set( SSC_PROJECT_NAME SSC ) 

#set( SSC_SOURCE_ROOT_DIR ${PROJECT_SOURCE_DIR}/.. )

SET( SSC_QT_MOC_HEADER_FILES
    sscView.h
    Utilities/sscAcceptanceBoxWidget.h
    Data/sscImage.h
    Data/sscImageTF3D.h
    Data/sscImageLUT2D.h
    Data/sscDataManager.h
    Tool/sscToolManager.h
    Tool/sscTool.h 
    Tool/sscDummyTool.h
    Tool/sscDummyToolManager.h
    Tool/sscManualTool.h
    Rep/sscSliceProxy.h
    Rep/sscRep.h
    Rep/sscSlicerRepSW.h
    Rep/sscToolRep3D.h
    Rep/sscProbeRep.h    
    Rep/sscImageBlenderProxy.h
    Rep/sscVolumetricRep.h    
    Rep/sscSlicePlaneRep.h
    Rep/sscProgressiveLODVolumetricRep.h
    Rep/sscBlendedSliceRep.h
    Rep/sscSlicedImageProxy.h
    Rep/sscSurfaceRep.h
        
    )

SET( SSC_HEADER_FILES
    ${SSC_QT_MOC_HEADER_FILES}
    sscRep.h 
    sscRepImpl.h 
    sscVolumeRep.h
    )


set( SSC_INCLUDE_DIRS
    ${SSC_SOURCE_DIR}/Code/  
    ${SSC_SOURCE_DIR}/Code/Data/
    ${SSC_SOURCE_DIR}/Code/Rep/
    ${SSC_SOURCE_DIR}/Code/Tool/
    ${SSC_SOURCE_DIR}/Code/Utilities/
    ) 

#set( SSC_INCLUDE_DIRS
#    .
#    Data
#    Rep
#    Tool
#    Utilities
#    ) 

set(SSC_LIBRARY_DIRS 
    )    

set( SSC_LIBRARIES 
    QVTK    
     )

# library
SET( SSC_SOURCE_FILES
    Utilities/sscBoundingBox3D.cpp
    Utilities/sscVector3D.cpp
    Utilities/sscTransform3D.cpp 
    Utilities/sscSliceComputer.cpp 
    Utilities/sscIndent.cpp
    Utilities/sscAcceptanceBoxWidget.cpp
    sscView.cpp
    Rep/sscRepImpl.cpp
    Rep/sscAxesRep.cpp
    Rep/sscSliceRep.cpp
    Rep/sscVolumetricRep.cpp
    Rep/sscProgressiveLODVolumetricRep.cpp    
    Rep/sscGeometricRep.cpp
    Rep/sscToolRep3D.cpp
    Rep/sscProbeRep.cpp
    Rep/sscSlicerRepSW.cpp
    Rep/sscSliceProxy.cpp
    Rep/sscSlicePlaneRep.cpp
    Rep/sscSlicedImageProxy.cpp
    Rep/sscBlendedSliceRep.cpp
    Rep/sscSurfaceRep.cpp
    Rep/sscImageBlenderProxy.cpp
    Data/sscData.cpp
    Data/sscImage.h
    Data/sscImage.cpp
    Data/sscImageTF3D.h
    Data/sscImageTF3D.cpp
    Data/sscImageLUT2D.cpp
    Data/sscMesh.cpp
    Data/sscDataManager.cpp
    Data/sscDataManagerImpl.cpp
    Tool/sscToolManager.cpp
    Tool/sscTool.h 
    Tool/sscDummyToolManager.cpp
    Tool/sscDummyTool.cpp
    Tool/sscManualTool.cpp
    Tool/sscProbeSector.cpp
	) 
	
# =========================================================
# =========================================================
#message( STATUS "processing project " ${SSC_PROJECT_NAME} "...")

## -  removed this, as -g is applied by cmake.
# add gcc definitions for all projects, except for Windows
#if (NOT WIN32)
#    add_definitions( -g  )
#endif(NOT WIN32)

# _USE_MATH_DEFINES Must be defined before math.h is included
if (WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif(WIN32)

# boost
find_package( Boost REQUIRED )

# =========================================================
# vtk
# =========================================================
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set( SSC_VTK_LIBRARIES 
    vtkIO 
    vtkRendering 
    vtkImaging 
    vtkCommon 
    vtkHybrid 
    vtkWidgets 
    vtkFiltering 
    vtkVolumeRendering  
    )

# =========================================================
# qt
# =========================================================
find_package( Qt4 REQUIRED )
set(QT_USE_QTNETWORK 1)
set(QT_USE_QTXML 1)
include( ${QT_USE_FILE} )

# clear variables; Qt appends to these vars
set( UI_HEADER_FILES )
set( MOC_HEADER_FILES )
set( GENERATED_RESOURCES )

QT4_WRAP_UI( UI_HEADER_FILES ${SSC_QT_UI_FILES} )
QT4_WRAP_CPP( MOC_HEADER_FILES ${SSC_QT_MOC_HEADER_FILES} )
QT4_ADD_RESOURCES( GENERATED_RESOURCES ${SSC_QT_RESOURCE_FILES} )

# =========================================================
# stuff
# =========================================================
# Configure the default SSC_DATA_ROOT for the location of SSCData.
FIND_PATH(SSC_DATA_ROOT sscData.readme 
  ${SSC_SOURCE_DIR}/Data
  $ENV{SSC_DATA_ROOT})
  
configure_file(
  "${SSC_SOURCE_DIR}/Code/Utilities/sscConfig.h.in"
  "${SSC_BINARY_DIR}/sscConfig.h"
  )

# =========================================================
# create output variables
# =========================================================

set( SSC_GENERATED_SOURCE_FILES
	${UI_HEADER_FILES} 
	${MOC_HEADER_FILES} 
	${SSC_SOURCE_FILES} 
	${GENERATED_RESOURCES} )

set( SSC_GENERATED_LIBRARIES
	${QT_LIBRARIES} 
	${SSC_VTK_LIBRARIES}
	${SSC_LIBRARIES} )
# =========================================================

include_directories( ${SSC_INCLUDE_DIRS} ${SSC_BINARY_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIRS} )
link_directories( ${SSC_LIBRARY_DIRS} )

set (BUILD_SHARED_LIBS OFF CACHE BOOL "build shared or static libs")

# =========================================================
# create the target
add_library( SSC ${SSC_GENERATED_SOURCE_FILES} )
target_link_libraries( SSC ${SSC_GENERATED_LIBRARIES} )
# =========================================================
# =========================================================

# =========================================================
# 
# Configure the file describing how to use SSC
#get_target_property (SSC_LIBRARY SSC LOCATION)
#set (SSC_INCLUDE ${PROJECT_BINARY_DIR}/include)

set (SSC_DIR ${SSC_BINARY_DIR} CACHE PATH "where to find the SSC cmake")

configure_file (
	"${PROJECT_SOURCE_DIR}/CMake/SSCConfig.cmake.in"
	"${PROJECT_BINARY_DIR}/SSCConfig.cmake"
	)
# Export our build settings and library dependencies
include (CMakeExportBuildSettings)
cmake_export_build_settings (${SSC_BINARY_DIR}/SSCBuildSettings.cmake)
export_library_dependencies (${SSC_BINARY_DIR}/SSCLibraryDepends.cmake)

# =========================================================

install( TARGETS SSC DESTINATION lib64 )
# =========================================================

