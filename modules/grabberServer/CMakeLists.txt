###########################################################
##               GrabberServer Application
###########################################################

project(GrabberServer)

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
set(QT_USE_QTXML TRUE)
set(QT_USE_QTNETWORK TRUE)
find_package(Qt4 REQUIRED)
if(QT_USE_FILE)
  include(${QT_USE_FILE})
else(QT_USE_FILE)
  set(QT_LIBRARIES   ${QT_QT_LIBRARY})
endif(QT_USE_FILE)

# VTK
#----------
find_package(VTK)
if(NOT VTK_DIR)
  message(FATAL_ERROR "Please set VTK_DIR.")
endif(NOT VTK_DIR)
include(${VTK_USE_FILE})

# ITK
#----------
find_package(ITK)
if(NOT ITK_DIR)
  message(FATAL_ERROR "Please set ITK_DIR.")
endif(NOT ITK_DIR)
include(${ITK_USE_FILE})

# IGSTK
#----------
find_package(IGSTK REQUIRED)
if(IGSTK_FOUND)
  include(${IGSTK_USE_FILE})    
endif(IGSTK_FOUND)

# OpenIGTLink
#----------
find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

# SSC
#----------
find_package(SSC PATHS "../../externals/ssc/CMake" REQUIRED)
include(${SSC_USE_FILE})

# APPLE libs
#----------
if(APPLE)

  ##Foundation
  find_library(FOUNDATION_LIBRARY Foundation)

  ##QtKit
  find_library(QTKIT_LIBRARY QtKit)

  ##Quartz
  find_library(QUARTZCORE_LIBRARY QuartzCore)

endif(APPLE)

# WINDOWS libs
#----------
if(WIN32)

    #TODO
    
endif(WIN32)

#------------------------------------------------------------------------------
# Qt - MOC
#------------------------------------------------------------------------------
set(${LIBNAME}_MOC_HDRS
    cxMainWindow.h
    cxGrabberServerWidget.h
    cxMacGrabberServerWidget.h
    cxWinGrabberServerWidget.h
    cxGrabber.h
    cxWinGrabber.h
    cxServer.h
    cxGrabberServer.h
    cxOpenIGTLinkSession.h
    ../grabberCommon/cxIGTLinkImageMessage.h
    cxWinGrabberServer.h
)

# After this call, cxProject_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx ...
if(APPLE)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS})
endif(APPLE)

if(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS} OPTIONS "-D_WINDOWS")
endif(WIN32)

#------------------------------------------------------------------------------
# Include
#------------------------------------------------------------------------------
set(SOURCES 
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
    ../grabberCommon/cxIGTLinkImageMessage.cpp
    ${${LIBNAME}_MOC_HDRS}
    ${${LIBNAME}_MOC_SRCS}
)

include_directories(
    .
    ${CMAKE_CURRENT_BINARY_DIR}/gui #so that is finds the generated ui files
    ${CustusX3_SOURCE_DIR}/source/ThirdParty/ultrasonix #to find ultrasonix files
)

#------------------------------------------------------------------------------
# Executable
#------------------------------------------------------------------------------
if(APPLE)
  add_executable(GrabberServer MACOSX_BUNDLE ${SOURCES})
endif(APPLE)

if(WIN32)
  add_executable(GrabberServer ${SOURCES})
endif(WIN32)

#------------------------------------------------------------------------------
# Link
#------------------------------------------------------------------------------
set( WIN_LINK_LIBRARIES
  ${ULTERIUS_LIBRARY}
  vtkOpenIGTUltrasound
)

set( MAC_LINK_LIBRARIES
  ${FOUNDATION_LIBRARY}
  ${QTKIT_LIBRARY}
  ${QUARTZCORE_LIBRARY}
)

set( ALL_LINK_LIBRARIES
  SSC
  OpenIGTLink
  ${QT_LIBRARIES}
)

if(WIN32)
    target_link_libraries( GrabberServer
      ${ALL_LINK_LIBRARIES}
      ${WIN_LINK_LIBRARIES}
    )
endif(WIN32)

if(APPLE)
    target_link_libraries( GrabberServer
      ${ALL_LINK_LIBRARIES}
      ${MAC_LINK_LIBRARIES}
    )
endif(APPLE)

#------------------------------------------------------------------------------
# Install
#------------------------------------------------------------------------------

# Variables
#----------
set(GRABBERSERVER_INSTALL_FOLDER_NAME "GrabberServer") # name of folder where all executables, config etc are assembled
if(WIN32)
    set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/GrabberServer.exe")
endif(WIN32)

if(APPLE)
    set(APPS ${CUSTUSX_INSTALL_FOLDER_NAME}/GrabberServer.app)
    #set(APPS "\${CMAKE_INSTALL_PREFIX}/${GRABBERSERVER_INSTALL_FOLDER_NAME}/GrabberServer.app")
endif(APPLE)

set(LIBS ${QT_LIBRARY_DIR})
set(DIRS ${QT_INCLUDES} ${QT_LIBRARY_DIRS} ${INCLUDE_DIRECTORIES})


# Install files
#----------
install(TARGETS GrabberServer
  BUNDLE DESTINATION ${GRABBERSERVER_INSTALL_FOLDER_NAME}
  RUNTIME DESTINATION bin
  #COMPONENT GrabberServer
  )

if(WIN32)

  #Installing required run time libs for Windows, msv*.dll
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
  include (InstallRequiredSystemLibraries)
  if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
      install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
        DESTINATION bin
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ
        GROUP_EXECUTE WORLD_READ
        #COMPONENT GrabberServer
        )
  endif(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
  
endif(WIN32)

#------------------------------------------------------------------------------
# Bundle utilities
#------------------------------------------------------------------------------
# See http://www.itk.org/Wiki/BundleUtilitiesExample for example on how to use bundle utilities

if(WIN32)

  install(CODE "
     include(BundleUtilities)
     fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\") "
     #COMPONENT GrabberServer
     )

endif(WIN32)
