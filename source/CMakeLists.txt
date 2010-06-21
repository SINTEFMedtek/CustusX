
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
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# ITK
#----------
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

# vtkINRIA3D
#----------
#FIND_PACKAGE(vtkINRIA3D REQUIRED)
#INCLUDE(${vtkINRIA3D_USE_FILE})

# IGSTK
#----------
FIND_PACKAGE(IGSTK REQUIRED)
INCLUDE(${IGSTK_USE_FILE})

# SSC
#----------
find_package (SSC PATHS "../externals/ssc/CMake" REQUIRED)
#if (SSC_FOUND)
	include (${SSC_USE_FILE})
#	link_directories (${SSC_DIR}/Code)
#endif (SSC_FOUND)
#message(STATUS "verdi av libfolders: " ${SSC_LIBRARY_DIRS} ) 
#----------

SET(SOURCES cxMain.cpp)

set(SUB_FOLDERS
    gui
    viewManager
    repManager
    registrationManager
    toolManager
    dataManager
    3DUSReconstruction
    UsReconstruction
    )
    
foreach (ITER ${SUB_FOLDERS})
    SET(LIB_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/${ITER} ${LIB_DIRECTORIES})    
endforeach (ITER ${SUB_FOLDERS})
set (LIB_DIRECTORIES 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LIB_DIRECTORIES}
    #${SSC_DIR}/Code 
    ${CMAKE_CURRENT_BINARY_DIR}/UsReconstruction/Thunder)

# rescope LIB_DIRECTORIES to parent scope
set(LIB_DIRECTORIES ${LIB_DIRECTORIES} PARENT_SCOPE)

INCLUDE_DIRECTORIES(
    .
    ${SUB_FOLDERS}
    /usr/local/include
    )
    

ADD_SUBDIRECTORY(gui)
ADD_SUBDIRECTORY(viewManager)
ADD_SUBDIRECTORY(repManager)
ADD_SUBDIRECTORY(toolManager)
ADD_SUBDIRECTORY(dataManager)
ADD_SUBDIRECTORY(registrationManager)
ADD_SUBDIRECTORY(UsReconstruction)
ADD_SUBDIRECTORY(apps)
IF(APPLE) # doesnt compile on ca's linux box - ??
    ADD_SUBDIRECTORY(3DUSReconstruction)
ENDIF(APPLE)

#--------------------------------------------------------------------------------
# For Apple set the icns file containing icons
IF(APPLE)
  # set how it shows up in the Info.plist file
  SET(MACOSX_BUNDLE_ICON_FILE application.icns) 
  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/gui/icons/application.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/gui/icons/application.icns)
ENDIF(APPLE)

IF(APPLE)
  SET(MACOSX_BUNDLE_INFO_STRING "CustusX")
  SET(MACOSX_BUNDLE_BUNDLE_NAME "CustusX")
  SET(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION_NUMBER}")
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
  #3DUSReconstruction
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
  #vtkDataManagement
  #vtkRenderingAddOn
  QVTK
  sscUsReconstructionLib  
  SSC
)

#----------
# LINK
TARGET_LINK_LIBRARIES( CustusX ${LINK_LIBRARIES} )


#--------------------------------------------------------------------------------
# Install the application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
install(TARGETS CustusX
    BUNDLE DESTINATION . #COMPONENT Runtime
    #RUNTIME DESTINATION bin #COMPONENT Runtime
    #COMPONENT application # crashes in installer
    )
      