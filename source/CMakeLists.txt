
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

# ITK
#----------
FIND_PACKAGE(ITK)
IF(NOT ITK_DIR)
  MESSAGE(FATAL_ERROR "Please set ITK_DIR.")
ENDIF(NOT ITK_DIR)
INCLUDE(${ITK_USE_FILE})

# vtkINRIA3D
#----------
FIND_PACKAGE(vtkINRIA3D REQUIRED)
INCLUDE(${vtkINRIA3D_USE_FILE})

# IGSTK
#----------
FIND_PACKAGE(IGSTK REQUIRED)
IF(IGSTK_FOUND)
  INCLUDE(${IGSTK_USE_FILE})
ENDIF(IGSTK_FOUND)

# SSC
#----------
find_package (SSC PATHS "../externals/ssc/CMake")
if (SSC_FOUND)
	include (${SSC_USE_FILE})
	#add_executable (testSSC testSSC.cpp)
	#target_link_libraries (testSSC SSC)
	link_directories (${SSC_DIR}/Code)
endif (SSC_FOUND)
message(STATUS "verdi av libfolders: " ${SSC_LIBRARY_DIRS} ) 
#----------

SET(SOURCES cxMain.cpp)

INCLUDE_DIRECTORIES(
	.
    gui
    viewManager
    repManager
    registrationManager
    toolManager
    dataManager
    3DUSReconstruction
    )
    

ADD_SUBDIRECTORY(gui)
ADD_SUBDIRECTORY(viewManager)
ADD_SUBDIRECTORY(repManager)
ADD_SUBDIRECTORY(toolManager)
ADD_SUBDIRECTORY(dataManager)
ADD_SUBDIRECTORY(registrationManager)
ADD_SUBDIRECTORY(3DUSReconstruction)


IF(APPLE)
  SET(MACOSX_BUNDLE_INFO_STRING "CustusX")
  SET(MACOSX_BUNDLE_BUNDLE_NAME "CustusX")
  SET(MACOSX_BUNDLE_BUNDLE_VERSION "3.${CustusX3_VERSION_MINOR}.${CustusX3_VERSION_PATCH}")
  SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${MACOSX_BUNDLE_BUNDLE_VERSION}")
  SET(MACOSX_BUNDLE_LONG_VERSION_STRING "CustusX ${MACOSX_BUNDLE_BUNDLE_VERSION}")
  SET(MACOSX_BUNDLE_COPYRIGHT "Copyright SINTEF\nAll rights reserved")
  ADD_EXECUTABLE(CustusX MACOSX_BUNDLE ${SOURCES} )
ELSE(APPLE)
  ADD_EXECUTABLE(CustusX ${SOURCES} )
ENDIF(APPLE)

SET ( LINK_LIBRARIES
  ${BOOST_LIBRARIES}
  ${QT_LIBRARIES}
  gui
  viewManager
  repManager
  registrationManager
  toolManager
  dataManager
  3DUSReconstruction
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

ADD_EXECUTABLE ( reconstruct apps/cxReconstructMain.cpp )
TARGET_LINK_LIBRARIES ( reconstruct
  ${LINK_LIBRARIES}
)