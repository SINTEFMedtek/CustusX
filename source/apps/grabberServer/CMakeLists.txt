###########################################################
##               GrabberServer Application
###########################################################

project(GrabberServer)

cmake_minimum_required(VERSION 2.6)

cx_define_option_from_boolean("BUILD_GRABBER_SERVER" "build grabber server" ${CX_APPLE})

if(BUILD_GRABBER_SERVER)

# APPLE libs
if(APPLE)
	find_library(FOUNDATION_LIBRARY Foundation)
	find_library(QTKIT_LIBRARY QtKit)
	find_library(QUARTZCORE_LIBRARY QuartzCore)
endif(APPLE)

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
		cxWinGrabberServer.h
)

QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS})

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
		cxWinGrabber.h
		cxWinGrabber.cpp
		cxServer.cpp
		cxGrabberServer.cpp
		cxWinGrabberServer.cpp
		cxMacGrabberServer.h
		cxMacGrabberServer.cpp
		cxOpenIGTLinkSession.cpp
		${${LIBNAME}_MOC_HDRS}
		${${LIBNAME}_MOC_SRCS}
)

include_directories(
		.
		${CMAKE_CURRENT_BINARY_DIR}/gui #so that is finds the generated ui files
		${CustusX3_SOURCE_DIR}/source/ThirdParty/ultrasonix #to find ultrasonix files
        ${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
)

if(CX_WINDOWS)
	set( WIN_LINK_LIBRARIES
		${ULTERIUS_LIBRARY}
		vtkOpenIGTUltrasound)
endif()

if(CX_APPLE)
	set( MAC_LINK_LIBRARIES
		${FOUNDATION_LIBRARY}
		${QTKIT_LIBRARY}
		${QUARTZCORE_LIBRARY})
endif()

set( ALL_LINK_LIBRARIES
	SSC
	OpenIGTLink
	cxOpenIGTLinkUtilities
	${QT_LIBRARIES})

#------------------------------------------------------------------------------
# Execute
#------------------------------------------------------------------------------

add_executable(GrabberServer ${SOURCES})

target_link_libraries( GrabberServer
	${ALL_LINK_LIBRARIES}
	${WIN_LINK_LIBRARIES}
	${MAC_LINK_LIBRARIES})

cx_install_target(GrabberServer)

endif(BUILD_GRABBER_SERVER)


