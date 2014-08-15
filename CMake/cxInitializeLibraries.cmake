# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
#
# CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
# code and binaries can only be used by SMT and those with explicit permission
# from SMT. CustusX shall not be distributed to anyone else.
#
# CustusX is a research tool. It is NOT intended for use or certified for use
# in a normal clinical setting. SMT does not take responsibility for its use
# in any way.
#
# See CustusX_License.txt for more information.


###############################################################################
# Initialize cppunit library
#
# Define variables:
#     CPPUNIT_FOUND
#     CPPUNIT_INCLUDE_DIR
#     CPPUNIT_LIBRARY
#     CPPUNIT_LIBRARIES
#     CPPUNIT_DEBUG_LIBRARIES
#
###############################################################################
macro(cx_initialize_cppunit)
    find_path(CPPUNIT_INCLUDE_DIR cppunit/TestCase.h
        /usr/local/include
        /usr/include)

    if(CX_WINDOWS)
        #CPP_UNIT not used on Windows
    else(CX_WINDOWS)
        find_library(CPPUNIT_LIBRARIES cppunit /opt/local/lib REQUIRED)
    endif(CX_WINDOWS)

    include_directories(${CPPUNIT_INCLUDE_DIR})
endmacro()

###############################################################################
# Initialize openCV library
#
# Define variables:
#    CX_USE_OpenCV : cache option
#
###############################################################################
macro(cx_initialize_opencv)
    if(CX_USE_OpenCV)
        find_package( OpenCV REQUIRED)
    else()
        find_package( OpenCV QUIET)
    endif()
    IF( OpenCV_FOUND )
        option(CX_USE_OpenCV "use OpenCV" ON)
    ELSE()
        option(CX_USE_OpenCV "use OpenCV" OFF)
    ENDIF()
endmacro()


###############################################################################
# Initialize openCL library
#
# Define variables:
#    SSC_USE_OpenCL : cache option
#
###############################################################################
macro(cx_initialize_opencl)
	if(SSC_USE_OpenCL)
		find_package( OpenCL REQUIRED)
	else()
		find_package( OpenCL QUIET)
	endif()
	IF( OPENCL_FOUND )
		option(SSC_USE_OpenCL "Use OpenCL" ON)
	ELSE()
		option(SSC_USE_OpenCL "Use OpenCL" OFF)
	ENDIF()

	if(OPENCL_FOUND)
	    add_definitions(-DSSC_USE_OpenCL)
		include_directories(${OPENCL_INCLUDE_DIRS})
	endif()
endmacro()

###############################################################################
# Initialize Level-Set-Segmentation library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_level_set_segmentation)
    find_package( Level-Set-Segmentation QUIET)
    if(Level-Set-Segmentation_FOUND)
        include(${Level-Set-Segmentation_USE_FILE})
    endif()
endmacro()

###############################################################################
# Initialize Tube-Segmentation-Framework library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_TSF)
    find_package( Tube-Segmentation-Framework QUIET)
    if(Tube-Segmentation-Framework_FOUND)
        include(${Tube-Segmentation-Framework_USE_FILE})
    endif()
endmacro()

###############################################################################
# Initialize OpenCLUtilityLibrary
#
# Find the package and run the include USE file.
#
# Define variables:
#    CX_USE_OPENCL_UTILITY : cache option
#
###############################################################################
macro(cx_initialize_OpenCLUtilityLibrary)
    if(CX_USE_OPENCL_UTILITY)
        find_package( OpenCLUtilityLibrary REQUIRED)
    else()
        find_package( OpenCLUtilityLibrary QUIET)
    endif()

    if(OpenCLUtilityLibrary_FOUND)
        option(CX_USE_OPENCL_UTILITY "use OpenCL UtilityLibrary" ON)
    else()
        option(CX_USE_OPENCL_UTILITY "use OpenCL UtilityLibrary" OFF)
    endif()

    if( CX_USE_OPENCL_UTILITY )
        ADD_DEFINITIONS(-DCX_USE_OPENCL_UTILITY)
        include(${OpenCLUtilityLibrary_USE_FILE})
    endif()
endmacro()


###############################################################################
# Initialize GEStreamer library
#
# Find the package and run the include USE file.
#
# Define variables:
#    CX_USE_ISB_GE : cache option
#
###############################################################################
macro(cx_initialize_ISB_GE)
    if(CX_USE_ISB_GE)
        find_package( GEStreamer REQUIRED)
    else()
        find_package( GEStreamer QUIET)
    endif()

    if(GEStreamer_FOUND)
        option(CX_USE_ISB_GE "use ISB GE scanner interface" ON)
    else()
        option(CX_USE_ISB_GE "use ISB GE scanner interface" OFF)
    endif()

    if(CX_USE_ISB_GE)
        ADD_DEFINITIONS(-DCX_USE_ISB_GE)
        include(${GEStreamer_USE_FILE})
    endif()
endmacro()


###############################################################################
# Initialize Boost library
# Find the package.
###############################################################################
macro(cx_initialize_Boost)
    find_package( Boost REQUIRED )
    include_directories(${Boost_INCLUDE_DIRS})
endmacro()

###############################################################################
# Initialize Qt library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_QT)
    set(QT_USE_QTXML TRUE)
    set(QT_USE_QTSQL TRUE)
    set(QT_USE_QTTEST TRUE)
    set(QT_USE_QTNETWORK 1)
    find_package(Qt4 REQUIRED)
    if(QT_USE_FILE)
        include(${QT_USE_FILE})
    else(QT_USE_FILE)
        set(QT_LIBRARIES  ${QT_QT_LIBRARY})
    endif(QT_USE_FILE)
endmacro()

###############################################################################
# Initialize VTK library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_VTK)
    find_package(VTK REQUIRED)
    include(${VTK_USE_FILE})
endmacro()

###############################################################################
# Initialize ITK library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_ITK)
    find_package(ITK REQUIRED)
    include(${ITK_USE_FILE})
endmacro()

###############################################################################
# Initialize OpenIGTLink library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_OpenIGTLink)
    find_package(OpenIGTLink REQUIRED)
    include(${OpenIGTLink_USE_FILE})
endmacro()

###############################################################################
# Initialize IGSTK library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_IGSTK)
    find_package(IGSTK REQUIRED)
    include(${IGSTK_USE_FILE})
endmacro()

###############################################################################
# Initialize CTK library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_CTK)
    find_package(CTK REQUIRED)
    include(${CTK_USE_FILE})

# override internal settings from CTK - otherwise the plugins will appear within ctk-named folders.
    set(CTK_INSTALL_PLUGIN_DIR "${CX_INSTALL_BINARY_DIR}")
    set(CTK_INSTALL_PLUGIN_INCLUDE_DIR "${CX_INSTALL_ROOT_DIR}/include/plugins")

endmacro()

###############################################################################
# Initialize Code coverage
#
# Add option SSC_USE_GCOV and generate variable SSC_GCOV_LIBRARY containing
# lib to link.
###############################################################################
macro(cx_initialize_coverage)
    # code coverage
    option(SSC_USE_GCOV "add gcov to enable coverage testing" OFF)
    if(SSC_USE_GCOV)
        message(STATUS "Building SSC with gcov code coverage support.")
        set(SSC_GCOV_LIBRARY )
        if(WIN32)
            message(ERROR "gcov not supported for WIN32")
        # needed on apple
        elseif(APPLE)
            set( SSC_GCOV_LIBRARY ${SSC_GCOV_LIBRARY} profile_rt )
        # needed on linux
        else(WIN32)
            set( SSC_GCOV_LIBRARY ${SSC_GCOV_LIBRARY} gcov)
        endif(WIN32)

        add_definitions(--coverage)
    endif()
endmacro()


###############################################################################
# Initialize eigen library
# Find the package and include folders
###############################################################################
macro(cx_initialize_Eigen)
    find_package(Eigen REQUIRED)
    #message(STATUS "Eigen version: " ${EIGEN_VERSION})
    #message(STATUS "Eigen inc: " ${EIGEN_INCLUDE_DIR})
    include_directories(${EIGEN_INCLUDE_DIR})
endmacro()

