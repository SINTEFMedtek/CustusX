#-------------------------------
# Linking
#-------------------------------
# link to ulterius only on when building 32 bit
# - ulterius lib is only available on windows as 32 bit
if(CX_WIN32)
    include_directories( ${ULTERIUS_INCLUDE_DIR} )
    set(WIN32_LINK_LIBRARIES
         ${ULTERIUS_LIBRARY}
    )
endif(CX_WIN32)

set(ALL_LINK_LIBRARIES
    ${BOOST_LIBRARIES}
    OpenIGTLink
)

cx_initialize_opencv()

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cxVideoServerConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cxVideoServerConfig.h"
  )
  
if(CX_USE_OpenCV)
    set(ALL_LINK_LIBRARIES
        ${ALL_LINK_LIBRARIES}
        ${OpenCV_LIBS}
    )
endif()


if(CX_WINDOWS)
    set(LINK_LIBRARIES
        ${WIN32_LINK_LIBRARIES}
        ${ALL_LINK_LIBRARIES}
    )
else()
    set(LINK_LIBRARIES
        ${ALL_LINK_LIBRARIES}
    )
endif()

#-------------------------------
# Qt -moccing
#-------------------------------
set(CX_QT_MOC_HEADER_FILES
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
    qt5_wrap_cpp(MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} OPTIONS "-D_WINDOWS")
else()
    qt5_wrap_cpp(MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES})
endif()


#-------------------------------
# Creating the executable
#-------------------------------
set(cxOpenIGTLinkServer_FILES
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

add_library( cxGrabber
    ${cxOpenIGTLinkServer_FILES} 
    ${MOC_HEADER_FILES} 
    )
target_include_directories(cxGrabber
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(cxGrabber
    PRIVATE
    ${LINK_LIBRARIES}

    PUBLIC
    cxOpenIGTLinkUtilities
    cxResource
    )

add_subdirectory(testing)
