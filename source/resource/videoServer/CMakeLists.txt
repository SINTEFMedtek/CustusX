cmake_minimum_required(VERSION 2.6)

#-------------------------------
# External libs
#-------------------------------

#-------------------------------
# Include directories
#-------------------------------
include_directories( ${CX_RESOURCE_INCLUDE_DIRECTORIES} )
include_directories(
        ${Boost_INCLUDE_DIRS}
     	${CustusX_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
        )

#-------------------------------
# Linking
#-------------------------------
# link to ulterius only on when building 32 bit
# - ulterius lib is only available on windows as 32 bit
if(CX_WIN32)

    include_directories( ${ULTERIUS_INCLUDE_DIR} )

    SET(WIN32_LINK_LIBRARIES
            ${ULTERIUS_LIBRARY}
    )
endif(CX_WIN32)

SET(ALL_LINK_LIBRARIES
	${BOOST_LIBRARIES}
	${QT_LIBRARIES}
	OpenIGTLink
    cxResource
    cxOpenIGTLinkUtilities
	)

cx_initialize_opencv()

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
else()
	SET(LINK_LIBRARIES
		${ALL_LINK_LIBRARIES}
	)
endif()

#-------------------------------
# Qt -moccing
#-------------------------------
SET( CX_QT_MOC_HEADER_FILES
		cxImageServer.h
		cxMHDImageStreamer.h
		cxImageStreamerOpenCV.h
		cxStreamer.h
		cxSender.h
		cxDirectlyLinkedSender.h
		SonixHelper.h
		cxtestSender.h
)

if(CX_WINDOWS)
	QT4_WRAP_CPP(MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} OPTIONS "-D_WINDOWS")
else()
	QT4_WRAP_CPP(MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES})
endif()


#-------------------------------
# Creating the executable
#-------------------------------
SET ( cxOpenIGTLinkServer_FILES
		cxImageServer.h
		cxImageServer.cpp
		cxCommandlineImageStreamerFactory.h
		cxCommandlineImageStreamerFactory.cpp
		cxMHDImageStreamer.h
		cxMHDImageStreamer.cpp
		cxImageStreamerOpenCV.h
		cxImageStreamerOpenCV.cpp
		cxImageStreamerSonix.h
		cxImageStreamerSonix.cpp
		cxStreamer.h
		cxStreamer.cpp
		cxSender.h
		cxSender.cpp
		cxDirectlyLinkedSender.h
		cxDirectlyLinkedSender.cpp
		cxSonixProbeFileReader.h
		cxSonixProbeFileReader.cpp
		
		vtkSonixVideoSource.h
		vtkSonixVideoSource.cxx
		vtkUltrasoundWin32Header.h
		cxFrame.h
		SonixHelper.h
		
		cxtestSender.h
		cxtestSender.cpp

)

add_library( cxGrabber
    ${cxOpenIGTLinkServer_FILES} 
    ${MOC_HEADER_FILES} 
    )
target_link_libraries(cxGrabber
    ${LINK_LIBRARIES}
    )

#Make GE defines available in source
#configure_file(
#  "${CMAKE_CURRENT_SOURCE_DIR}/geConfig.h.in"
#  "${CMAKE_CURRENT_BINARY_DIR}/geConfig.h" )
#include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory(testing)
