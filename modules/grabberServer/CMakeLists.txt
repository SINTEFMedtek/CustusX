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

#Foundation
#----------
IF(APPLE)
  FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)
ENDIF(APPLE)

#QtKit
#----------
IF(APPLE)
  FIND_LIBRARY(QTKIT_LIBRARY QtKit)
ENDIF(APPLE)

#QuickTime
#---------
#IF(APPLE)
#  FIND_LIBRARY(QUICKTIME_LIBRARY QuickTime)
#ENDIF(APPLE)

#Quartz
#------
IF(APPLE)
  FIND_LIBRARY(QUARTZCORE_LIBRARY QuartzCore)
ENDIF(APPLE)

#------------------------------------------------------------------------------
# Qt - MOC
#------------------------------------------------------------------------------
SET(${LIBNAME}_MOC_HDRS
    cxMainWindow.h
    cxGrabberServerWidget.h
    cxGrabberServerPropertiesWidget.h
    cxGrabber.h
    cxServer.h
    cxGrabberServer.h
    cxOpenIGTLinkSession.h
)

# After this call, cxProject_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx ...
IF(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS} ${QTUI_HEADERFILES} OPTIONS "-D_WINDOWS")
ELSE(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS})
ENDIF(WIN32)

#------------------------------------------------------------------------------
# Include
#------------------------------------------------------------------------------
SET(SOURCES 
    main.cpp
    cxMainWindow.cpp
    cxGrabberServerWidget.cpp
    cxGrabberServerPropertiesWidget.cpp
    cxGrabber.mm
    cxServer.cpp
    cxGrabberServer.cpp
    cxOpenIGTLinkSession.cpp
    ${${LIBNAME}_MOC_HDRS}
    ${${LIBNAME}_MOC_SRCS}
)

INCLUDE_DIRECTORIES(
    .
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}/gui #so that is finds the generated ui files
)

#------------------------------------------------------------------------------
# Executable
#------------------------------------------------------------------------------
ADD_EXECUTABLE(GrabberServer MACOSX_BUNDLE ${SOURCES})

#------------------------------------------------------------------------------
# Link
#------------------------------------------------------------------------------
SET ( LINK_LIBRARIES
  SSC
  OpenIGTLink
  ${QT_LIBRARIES}
  ${FOUNDATION_LIBRARY}
  ${QTKIT_LIBRARY}
  #${QUICKTIME_LIBRARY}
  ${QUARTZCORE_LIBRARY}
)

TARGET_LINK_LIBRARIES( GrabberServer
  ${LINK_LIBRARIES}
)

#------------------------------------------------------------------------------
# Install
#------------------------------------------------------------------------------
install(TARGETS GrabberServer
    BUNDLE DESTINATION ${INSTALL_FOLDER_NAME}
    RUNTIME DESTINATION bin
    )