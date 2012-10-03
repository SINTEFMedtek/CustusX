cmake_minimum_required(VERSION 2.6)

#-------------------------------
# External libs
#-------------------------------

# Boost
#----------

find_package( Boost REQUIRED )

# openCV
#----------
if (USE_OpenCV)
    find_package( OpenCV REQUIRED)
else ()
    find_package( OpenCV QUIET)
endif ()

IF( OpenCV_FOUND )
    option(USE_OpenCV "use OpenCV" ON)
ELSE()
    option(USE_OpenCV "use OpenCV" OFF)
ENDIF()

if (USE_OpenCV)
    ADD_DEFINITIONS(-DUSE_OpenCV)
endif()

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
	)

if(USE_OpenCV)
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
		../grabberCommon/cxIGTLinkUSStatusMessage.h
		../grabberCommon/cxIGTLinkUSStatusMessage.cpp
		../grabberCommon/cxIGTLinkImageMessage.h
		../grabberCommon/cxIGTLinkImageMessage.cpp
		../grabberCommon/igtl_us_status.h
		../grabberCommon/igtl_us_status.c
)

if(CX_WIN32)
	set(ExecutableName "UltrasonixServer") # windows 32 bit version is made for ultrasonix communication
else()
	set(ExecutableName "OpenIGTLinkServer")
endif()

add_executable( ${ExecutableName}
    main.cpp 
    ${cxOpenIGTLinkServer_FILES} 
    ${MOC_HEADER_FILES} 
    )

target_link_libraries(${ExecutableName}
    ${LINK_LIBRARIES}
    )

#-------------------------------
# Install files
#-------------------------------
if (CX_WINDOWS OR CX_LINUX)
    install(TARGETS ${ExecutableName}
        BUNDLE DESTINATION ${CX_INSTALL_ROOT_DIR}
        RUNTIME DESTINATION ${CX_INSTALL_ROOT_DIR}/bin
        )
endif()

if(CX_WINDOWS)
	#Installing required run time libs for Windows, msv*.dll
	set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
	include (InstallRequiredSystemLibraries)
	if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
			install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
				DESTINATION bin
				PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ
				GROUP_EXECUTE WORLD_READ
				)
	endif(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
	
endif(CX_WINDOWS)

#------------------------------------------------------------------------------
# Bundle utilities
#------------------------------------------------------------------------------
# See http://www.itk.org/Wiki/BundleUtilitiesExample for example on how to use bundle utilities

# Linux
#----------
if (CX_LINUX)
	set(APPS "\${CMAKE_INSTALL_PREFIX}/${CX_INSTALL_ROOT_DIR}/bin/OpenIGTLinkServer")
	set(LIBS "")
	set(DIRS 
			${QT_INCLUDES} 
			${QT_LIBRARY_DIRS}
			${VTK_LIBRARY_DIRS}
			${SSC_BINARY_DIR}/Code
			${OpenIGTLink_LIBRARY_DIRS}
			${OpenCV_LIB_DIR}
			)
			
	install(CODE "
		 include(BundleUtilities)
		 fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\") "
		 )      
			
endif (CX_LINUX)

# Windows
#----------
if (CX_WINDOWS)
	set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${ExecutableName}.exe")
	set(LIBS
			)
	set(DIRS 
			${QT_INCLUDES} 
			${QT_LIBRARY_DIRS}
			${QT_BINARY_DIR}
			${VTK_LIBRARY_DIRS}
			${SSC_BINARY_DIR}/Code
			${OpenIGTLink_LIBRARY_DIRS}
			)
	if(USE_OpenCV)
		set (DIRS
				${DIRS}
				${OpenCV_LIB_DIR}
				)
	endif()
	#message (status "APPS: "${APPS})
	#message (status "LIBS: "${LIBS})
	#message (status "DIRS: "${DIRS})
	#message (status "QT_BINARY_DIR: "${QT_BINARY_DIR})
	install(CODE "
		 include(BundleUtilities)
		 fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\") "
		 )      
			
endif (CX_WINDOWS)