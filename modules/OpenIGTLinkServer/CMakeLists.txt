cmake_minimum_required(VERSION 2.6)

# Boost
#----------
find_package( Boost REQUIRED )

# openCV
#----------
if (USE_OpenCV)
    find_package( OpenCV REQUIRED)
else (USE_OpenCV)
    find_package( OpenCV QUIET)
endif (USE_OpenCV)

IF( OpenCV_FOUND )
    option(USE_OpenCV "use OpenCV" ON)
ELSE( OpenCV_FOUND )
    option(USE_OpenCV "use OpenCV" OFF)
ENDIF( OpenCV_FOUND )

if (USE_OpenCV)
  ADD_DEFINITIONS(-DUSE_OpenCV="${USE_OpenCV}")
endif(USE_OpenCV)

# VTK
#----------
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# Qt
#----------
# find and setup Qt4 for this project
SET (QT_USE_QTXML TRUE)
find_package(Qt4 REQUIRED)
set(QT_USE_QTNETWORK 1)
IF(QT_USE_FILE)
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
  SET(QT_LIBRARIES   ${QT_QT_LIBRARY})
ENDIF(QT_USE_FILE)

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})


SET ( LINK_LIBRARIES
  ${BOOST_LIBRARIES}
  ${QT_LIBRARIES}
  OpenIGTLink
  )

SET( CX_QT_MOC_HEADER_FILES
    cxImageServer.h
    cxImageSession.h
    cxImageSenderFile.h
    cxImageSenderOpenCV.h
    cxImageSenderFactory.h
)

include_directories(${Boost_INCLUDE_DIRS})
#link_directories(${Boost_INCLUDE_DIRS})

SET ( cxOpenIGTLinkServer_FILES
    cxImageServer.h
    cxImageServer.cpp
    cxImageSession.h
    cxImageSession.cpp 
    cxImageSenderFactory.h
    cxImageSenderFactory.cpp
    cxImageSenderFile.h
    cxImageSenderFile.cpp
    cxImageSenderOpenCV.h
    cxImageSenderOpenCV.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

add_executable( cxOpenIGTLinkServer main.cpp ${cxOpenIGTLinkServer_FILES} ${MOC_HEADER_FILES} )
target_link_libraries(cxOpenIGTLinkServer  ${LINK_LIBRARIES}     
    ${OpenCV_LIBS}
    vtkIO 
    vtkRendering 
    vtkImaging 
    vtkCommon 
    vtkHybrid 
    vtkWidgets 
    vtkFiltering 
    vtkVolumeRendering  )
      