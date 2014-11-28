
#-------------------------------
# Include directories
#-------------------------------
include_directories( ${CX_RESOURCE_INCLUDE_DIRECTORIES} )
include_directories(
        ${Boost_INCLUDE_DIRS}
     	${CustusX_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
     	${CustusX_BINARY_DIR}/source/resource/OpenIGTLinkUtilities
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
        cxSenderImpl.h
        cxGrabberSenderQTcpSocket.h
		cxDirectlyLinkedSender.h
		SonixHelper.h
		cxtestSender.h
)

cx_remove_duplicate_include_directories()
if(CX_WINDOWS)
	QT5_WRAP_CPP(MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} OPTIONS "-D_WINDOWS")
else()
	QT5_WRAP_CPP(MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES})
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
        cxSenderImpl.h
        cxSenderImpl.cpp
        cxGrabberSenderQTcpSocket.h
        cxGrabberSenderQTcpSocket.cpp
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

cx_create_export_header("cxGrabber")
cx_remove_duplicate_include_directories()
add_library( cxGrabber
    ${cxOpenIGTLinkServer_FILES} 
    ${MOC_HEADER_FILES} 
    )
target_link_libraries(cxGrabber
    ${LINK_LIBRARIES}
    )

add_subdirectory(testing)
