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
        find_library(CPPUNIT_LIBRARY cppunit
            ${CPPUNIT_INCLUDE_DIR}/../lib
            /usr/local/lib
            /usr/lib)
        find_library(CPPUNIT_DEBUG_LIBRARY cppunitd
            ${CPPUNIT_INCLUDE_DIR}/../lib
            /usr/local/lib
            /usr/lib)
    else(CX_WINDOWS)
        find_library(CPPUNIT_LIBRARIES cppunit /opt/local/lib REQUIRED)
    endif(CX_WINDOWS)

    if(CPPUNIT_INCLUDE_DIR)
        if(CPPUNIT_LIBRARY)
            set(CPPUNIT_FOUND "YES")
            set(CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
            set(CPPUNIT_DEBUG_LIBRARIES ${CPPUNIT_DEBUG_LIBRARY} ${CMAKE_DL_LIBS})
        endif(CPPUNIT_LIBRARY)
    endif(CPPUNIT_INCLUDE_DIR)
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
# Initialize Tube-Segmentation-Framework library
#
# Find the package and run the include USE file.
#
# Define variables:
#    CX_USE_TSF : cache option
#
###############################################################################
macro(cx_initialize_TSF)
    # TSF (Tube-Segmentation-Framework)
    #----------
    if(CX_USE_TSF)
        find_package(Tube-Segmentation-Framework PATHS "${CustusX3_SOURCE_DIR}/../../Tube-Segmentation-Framework/Tube-Segmentation-Framework/CMake" REQUIRED)
    endif()

    IF( Tube-Segmentation-Framework_FOUND )
        option(CX_USE_TSF "use TSF (Tube-Segmentation-Framework)" ON)
        ADD_DEFINITIONS(-DCX_USE_TSF)
        include(${Tube-Segmentation-Framework_USE_FILE})
    ELSE()
        option(CX_USE_TSF "use TSF (Tube-Segmentation-Framework)" OFF)
    ENDIF()
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
endmacro()

###############################################################################
# Initialize Qt library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_QT)
    set(QT_USE_QTXML TRUE)
    set(QT_USE_QTTEST TRUE)
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
# Initialize SSC library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_SSC)
    find_package(SSC PATHS "../externals/ssc/CMake" REQUIRED)
    include(${SSC_USE_FILE})
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
endmacro()

