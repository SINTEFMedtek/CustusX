cmake_minimum_required(VERSION 2.6)

#-------------------------------
# External libs
#-------------------------------

# Boost
#----------

find_package( Boost REQUIRED )

# GEStreamer
#----------
option(CX_USE_ISB_GE "use ISB GE scanner interface" OFF)
if (CX_USE_ISB_GE)
    ADD_DEFINITIONS(-DCX_USE_ISB_GE)
    #include_directories( ${CustusX3_SOURCE_DIR}/../../ISB_DataStreaming/ISB_DataStreaming/vtkDataStreamClient )

    find_package(GEStreamer REQUIRED)
    include(${GEStreamer_USE_FILE})
endif()


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

# OpenIGTLink
#----------
find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

# SSC
#----------
find_package(SSC PATHS "../../externals/ssc/CMake" REQUIRED)
include(${SSC_USE_FILE})

#-------------------------------
# Include directories
#-------------------------------
include_directories(
        ${Boost_INCLUDE_DIRS}
        ${CustusX3_SOURCE_DIR}/source/ThirdParty/ultrasonix #to find ultrasonix files
        ${CustusX3_SOURCE_DIR}/source/resource/settings
        ${CustusX3_BINARY_DIR}/source/resource/settings
        ${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
        #"C:/Program Files/Microsoft SDKs/Windows/v7.1/Samples/multimedia/directshow/baseclasses"
        )

#-------------------------------
# Linking
#-------------------------------

#link_directories( "C:/Program Files/Microsoft SDKs/Windows/v7.1/Lib" )

# link to ulterius only on when building 32 bit
# - ulterius lib is only available on windows as 32 bit
if(CX_WIN32)
    SET(WIN32_LINK_LIBRARIES
            ${ULTERIUS_LIBRARY}
            vtkOpenIGTUltrasound
            #STRMBASE
    )
endif(CX_WIN32)

SET(ALL_LINK_LIBRARIES
	SSC
	${BOOST_LIBRARIES}
	${QT_LIBRARIES}
	${GEStreamer_LIBRARIES}
	OpenIGTLink
	vtkIO 
	vtkRendering 
	vtkImaging 
	vtkCommon 
	vtkHybrid 
	vtkWidgets 
	vtkFiltering 
	vtkVolumeRendering 
    cxResource
    cxOpenIGTLinkUtilities
	)

if(CX_USE_OpenCV)
	set (ALL_LINK_LIBRARIES
			${ALL_LINK_LIBRARIES}
			${OpenCV_LIBS}
			)
endif()


if(CX_WINDOWS)
	SET(LINK_LIBRARIES
		${WIN32_LINK_LIBRARIES}
		${ALL_LINK_LIBRARIES}
	)
	else(CX_WINDOWS)
		SET(LINK_LIBRARIES
			${ALL_LINK_LIBRARIES}
	)
endif(CX_WINDOWS)

#-------------------------------
# Qt -moccing
#-------------------------------
SET( CX_QT_MOC_HEADER_FILES
		cxImageServer.h
		cxImageSenderFile.h
		cxImageSenderSonix.h
		cxImageSenderOpenCV.h
		cxImageSenderGE.h
		cxImageSender.h
		cxGrabberSender.h
)

#QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

if(CX_WINDOWS)
	QT4_WRAP_CPP(MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} OPTIONS "-D_WINDOWS")
else()
	QT4_WRAP_CPP(MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES})
endif(CX_WINDOWS)


#-------------------------------
# Creating the executable
#-------------------------------
SET ( cxOpenIGTLinkServer_FILES
		cxImageServer.h
		cxImageServer.cpp
		cxImageSenderFactory.h
		cxImageSenderFactory.cpp
		cxImageSenderFile.h
		cxImageSenderFile.cpp
		cxImageSenderOpenCV.h
		cxImageSenderOpenCV.cpp
		cxImageSenderSonix.h
		cxImageSenderSonix.cpp
		cxImageSenderGE.h
		cxImageSenderGE.cpp
		cxImageSender.h
		cxGrabberSender.h
		cxGrabberSender.cpp

)

add_library( cxGrabber
    ${cxOpenIGTLinkServer_FILES} 
    ${MOC_HEADER_FILES} 
    )
target_link_libraries(cxGrabber
    ${LINK_LIBRARIES}
    )

#Make GE defines available in source
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/geConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/geConfig.h" )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )


add_subdirectory(testing)