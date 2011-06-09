###########################################################
##               GrabberServer Application
###########################################################

PROJECT(GrabberServer)

cmake_minimum_required(VERSION 2.6)

#------------------------------------------------------------------------------
# External libraries
#------------------------------------------------------------------------------

# Boost
#----------
find_package( Boost REQUIRED )

# Qt
#----------
# find and setup Qt4 for this project
SET (QT_USE_QTXML TRUE)
SET (QT_USE_QTNETWORK TRUE)
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

# IGSTK
#----------
FIND_PACKAGE(IGSTK REQUIRED)
IF(IGSTK_FOUND)
  INCLUDE(${IGSTK_USE_FILE})    
ENDIF(IGSTK_FOUND)

# OpenIGTLink
#----------
find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

# SSC
#----------
find_package (SSC PATHS "../../externals/ssc/CMake" REQUIRED)
include (${SSC_USE_FILE})

# APPLE libs
#----------
IF(APPLE)

  ##Foundation
  FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)

  ##QtKit
  FIND_LIBRARY(QTKIT_LIBRARY QtKit)

  ##Quartz
  FIND_LIBRARY(QUARTZCORE_LIBRARY QuartzCore)

ENDIF(APPLE)

# WINDOWS libs
#----------
IF(WIN32)

ENDIF(WIN32)

#------------------------------------------------------------------------------
# Qt - MOC
#------------------------------------------------------------------------------
SET(${LIBNAME}_MOC_HDRS
    cxMainWindow.h
    cxGrabberServerWidget.h
    cxMacGrabberServerWidget.h
    cxWinGrabberServerWidget.h
    cxGrabber.h
    cxWinGrabber.h
    cxServer.h
    cxGrabberServer.h
    cxOpenIGTLinkSession.h
    cxIGTLinkImageMessage.h
    cxWinGrabberServer.h
)

# After this call, cxProject_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx ...
IF(APPLE)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS})
ENDIF(APPLE)

IF(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS} OPTIONS "-D_WINDOWS")
ENDIF(WIN32)

#------------------------------------------------------------------------------
# Include
#------------------------------------------------------------------------------
SET(SOURCES 
    main.cpp
    cxMainWindow.cpp
    cxGrabberServerWidget.cpp
    cxMacGrabberServerWidget.cpp
    cxWinGrabberServerWidget.cpp
    cxGrabber.cpp
    cxMacGrabber.h
    cxMacGrabber.mm
    cxWinGrabber.cpp
    cxServer.cpp
    cxGrabberServer.cpp
    cxWinGrabberServer.cpp
    cxMacGrabberServer.h
    cxMacGrabberServer.cpp
    cxOpenIGTLinkSession.cpp
    cxIGTLinkImageMessage.cpp
    ${${LIBNAME}_MOC_HDRS}
    ${${LIBNAME}_MOC_SRCS}
)

INCLUDE_DIRECTORIES(
    .
    ${CMAKE_CURRENT_BINARY_DIR}/gui #so that is finds the generated ui files
    ${CustusX3_SOURCE_DIR}/source/ThirdParty/ultrasonix #to find ultrasonix files
)

#------------------------------------------------------------------------------
# Executable
#------------------------------------------------------------------------------
IF(APPLE)
  ADD_EXECUTABLE(GrabberServer MACOSX_BUNDLE ${SOURCES})
ENDIF(APPLE)

IF(WIN32)
  ADD_EXECUTABLE(GrabberServer ${SOURCES})
ENDIF(WIN32)

#------------------------------------------------------------------------------
# Link
#------------------------------------------------------------------------------
SET ( WIN_LINK_LIBRARIES
  ${ULTERIUS_LIBRARY}
  vtkOpenIGTUltrasound
)

SET ( MAC_LINK_LIBRARIES
  ${FOUNDATION_LIBRARY}
  ${QTKIT_LIBRARY}
  ${QUARTZCORE_LIBRARY}
)

SET ( ALL_LINK_LIBRARIES
  SSC
  OpenIGTLink
  ${QT_LIBRARIES}
)

IF(WIN32)
TARGET_LINK_LIBRARIES( GrabberServer
  ${ALL_LINK_LIBRARIES}
  ${WIN_LINK_LIBRARIES}
)
ENDIF(WIN32)

IF(APPLE)
TARGET_LINK_LIBRARIES( GrabberServer
  ${ALL_LINK_LIBRARIES}
  ${MAC_LINK_LIBRARIES}
)
ENDIF(APPLE)

#------------------------------------------------------------------------------
# Install
#------------------------------------------------------------------------------
# See http://www.itk.org/Wiki/BundleUtilitiesExample for example on how to use bundle utilities

IF(WIN32)

  install(TARGETS GrabberServer
      BUNDLE DESTINATION ${INSTALL_FOLDER_NAME}
      RUNTIME DESTINATION bin
      COMPONENT GrabberServer
      )

  include (InstallRequiredSystemLibraries)

  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/GrabberServer.exe")
  SET(LIBS ${QT_LIBRARY_DIR})
  SET(DIRS ${QT_INCLUDES} ${QT_LIBRARY_DIRS} ${INCLUDE_DIRECTORIES})

  INSTALL(CODE "
     include(BundleUtilities)
     fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\") "
     COMPONENT GrabberServer
     )

ENDIF(WIN32)
