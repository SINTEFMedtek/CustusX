
cmake_minimum_required(VERSION 2.6)

# Boost
#----------
find_package( Boost REQUIRED )

# Qt
#----------
# find and setup Qt4 for this project
SET (QT_USE_QTXML TRUE)
find_package(Qt4 REQUIRED)
IF(QT_USE_FILE)
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
  SET(QT_LIBRARIES   ${QT_QT_LIBRARY})
ENDIF(QT_USE_FILE)

# VTK
#----------
FIND_PACKAGE(VTK)
IF(NOT VTK_DIR)
  MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(NOT VTK_DIR)
INCLUDE(${VTK_USE_FILE})

# vtkINRIA3D
#----------
FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})

# ITK
#----------
FIND_PACKAGE(ITK)
IF(NOT ITK_DIR)
  MESSAGE(FATAL_ERROR "Please set ITK_DIR.")
ENDIF(NOT ITK_DIR)
INCLUDE(${ITK_USE_FILE})

# IGSTK
#----------
FIND_PACKAGE(IGSTK REQUIRED)
IF(IGSTK_FOUND)
  INCLUDE(${IGSTK_USE_FILE})
ENDIF(IGSTK_FOUND)

# SSC
#----------
find_package (SSC)
if (SSC_FOUND)
	include (${SSC_USE_FILE})
	#add_executable (testSSC testSSC.cpp)
	#target_link_libraries (testSSC SSC)
endif (SSC_FOUND)
message(STATUS "verdi av libfolders: " ${SSC_LIBRARY_DIRS} ) 
#----------

SET(SOURCES cxMain.cpp)

INCLUDE_DIRECTORIES(
    gui
    viewManager
    repManager
    toolManager
    )
    

ADD_SUBDIRECTORY(gui)
ADD_SUBDIRECTORY(viewManager)
ADD_SUBDIRECTORY(repManager)
ADD_SUBDIRECTORY(toolManager)

ADD_EXECUTABLE(CustusX MACOSX_BUNDLE ${SOURCES} )

SET ( LINK_LIBRARIES
  ${BOOST_LIBRARIES}
  ${QT_LIBRARIES}
  gui
  viewManager
  repManager
  toolManager
  vtkRendering
  vtkWidgets
  vtkGraphics
  vtkIO
  vtkCommon
  vtkImaging
  vtkFiltering
  vtkHybrid
  vtkVolumeRendering
#  ITKCommon
#  ITKBasicFilters
#  ITKIO
  IGSTK
  vtkDataManagement
  vtkRenderingAddOn
  QVTK
  SSC
)

# LINK
#----------
TARGET_LINK_LIBRARIES( CustusX
  ${LINK_LIBRARIES}
)
