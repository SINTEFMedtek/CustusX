# =========================================================
# =========================================================
# Helper function for creating Sonowand executables.
# Include the file into your project using include().
#
# Usage:
# Define the variables
#    SNW_PROJECT_NAME
#    SNW_SOURCE_ROOT_DIR
#    SNW_HEADER_FILES
#    SNW_SOURCE_FILES
#    SNW_INCLUDE_DIRS
#    SNW_LIBRARY_DIRS
#    SNW_LIBRARIES
#    SNW_QT_UI_FILES
#    SNW_QT_MOC_HEADER_FILES
#    SNW_QT_RESOURCE_FILES
#
# The file will connect to boost, vtk, qt, and perform the boilerplate
# operations needed, such as generating Qt moc files and setting directories.        
#
# Output:
#    SNW_GENERATED_SOURCE_FILES
#    SNW_GENERATED_LIBRARIES
#
#    Use these two variables to call add_executable/add_library
#    and then call target_link_libraries
#
#
# =========================================================
# =========================================================

message( STATUS "processing project " ${SNW_PROJECT_NAME} "...")

# add gcc definitions for all projects
add_definitions( -g  )

# boost
find_package( Boost REQUIRED )

# =========================================================
# vtk
# =========================================================
find_package(VTK REQUIRED)
if(NOT VTK_USE_RENDERING)
  message(FATAL_ERROR ${SNW_PROJECT_NAME} requires VTK_USE_RENDERING.)
endif(NOT VTK_USE_RENDERING)
include(${VTK_USE_FILE})

set( SNW_VTK_LIBRARIES 
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
if( QT_USE_FILE )
  include( ${QT_USE_FILE} )
else( QT_USE_FILE )
  set( QT_LIBRARIES ${QT_QT_LIBRARY} )
endif( QT_USE_FILE )


include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIRS} )
link_directories( ${SNW_LIBRARY_DIRS} )

# clear variables; Qt appends to these vars
set( UI_HEADER_FILES )
set( MOC_HEADER_FILES )
set( GENERATED_RESOURCES )

QT4_WRAP_UI( UI_HEADER_FILES ${SNW_QT_UI_FILES} )
QT4_WRAP_CPP( MOC_HEADER_FILES ${SNW_QT_MOC_HEADER_FILES} )
QT4_ADD_RESOURCES( GENERATED_RESOURCES ${SNW_QT_RESOURCE_FILES} )

# =========================================================
# stuff
# =========================================================
# Configure the default SSC_DATA_ROOT for the location of SSCData.
FIND_PATH(SSC_DATA_ROOT SSCData.readme 
  ${SSC_SOURCE_DIR}/TestData 
  $ENV{SSC_DATA_ROOT})
  
configure_file(
  "${SSC_SOURCE_DIR}/Utilities/sscConfig.h.in"
  "${SSC_BINARY_DIR}/sscConfig.h"
  )

# =========================================================
# create output variables
# =========================================================

set( SNW_GENERATED_SOURCE_FILES
	${UI_HEADER_FILES} 
	${MOC_HEADER_FILES} 
	${SNW_SOURCE_FILES} 
	${GENERATED_RESOURCES} )

set( SNW_GENERATED_LIBRARIES
	${QT_LIBRARIES} 
	${SNW_VTK_LIBRARIES}
	${SNW_LIBRARIES} )
# =========================================================
