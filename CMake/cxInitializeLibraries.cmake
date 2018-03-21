# =========================================================================
# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (c) SINTEF Department of Medical Technology.
# All rights reserved.
#
# CustusX is released under a BSD 3-Clause license.
#
# See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
# =========================================================================


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
# Initialize FAST library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_FAST)
    find_package(FAST QUIET)
    if(FAST_FOUND)
        message("=========================================")
        message("FAST was found!!!!!")
        message("${FAST_SOURCE_DIR}")
        message("${FAST_INCLUDE_DIRS}")
        message("${FAST_LIBRARY_DIRS}")
        message("${FAST_LIBRARY}")
        message("=========================================")
        include(${FAST_USE_FILE})
    else()
        message("=========================================")
        message("FAST was NOT found")
        message("=========================================")
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
    find_package(Qt5Core REQUIRED) # attempt to remove warning in cmake
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5OpenGL REQUIRED)
    find_package(Qt5Xml REQUIRED)
    find_package(Qt5Help REQUIRED)
    find_package(Qt5Sql REQUIRED)
    find_package(Qt5Test REQUIRED)
    find_package(Qt5Network REQUIRED)
    find_package(Qt5Concurrent REQUIRED) # attempt to remove warning in cmake
    find_package(Qt5Multimedia REQUIRED)
    find_package(Qt5OpenGL REQUIRED)
endmacro()

###############################################################################
# Initialize VTK library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_VTK)
	# this path add should be automatic, but fails at least on ca/macosx10.8
	set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${VTK_DIR}/lib)

	# vtk consist of (as of 6.1) 120 libs. This explicit inclusion brings is down to about half.
	# gives about 15% compile speed increase
	#
	# See http://www.vtk.org/Wiki/VTK/Build_System_Migration
	#
	find_package(VTK COMPONENTS
		vtkCommonCore
		vtkRenderingCore
		vtkRenderingQt
		vtkInteractionStyle
		vtkRenderingVolume
		vtkIOGeometry vtkIOLegacy vtkIOMINC vtkIOXML
		vtkIOXMLParser
		vtkFiltersModeling
		vtkInteractionWidgets
		vtkParallelCore
		vtkFiltersParallel
		vtkImagingMath vtkImagingMorphological vtkImagingColor vtkImagingStatistics
		vtkRenderingOpenGL2
		vtkRenderingVolumeOpenGL2
		NO_MODULE)


#    find_package(VTK REQUIRED) # import all libs - try this for debugging lib includes
    include(${VTK_USE_FILE})
endmacro()

###############################################################################
# Initialize ITK library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_ITK )
	# we use only a small part of ITK. Adding only a few libs reduces cmd-line-
	# length problems on Win, and reduces built time:
	find_package(ITK COMPONENTS
		ITKCommon ITKIOVTK ITKVTK ITKIOImageBase ITKMathematicalMorphology
		ITKSmoothing ITKRegionGrowing ITKBinaryMathematicalMorphology
		ITKRegistrationCommon ITKQuadEdgeMesh ITKIOMesh ITKQuadEdgeMeshFiltering
		ITKMetaIO ITKIOMeta
		)
    include(${ITK_USE_FILE})
	#find_package(ITK REQUIRED) # import all libs - try this for debugging lib includes
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
# Initialize OpenIGTLinkIO library
# Find the package.
###############################################################################
macro(cx_initialize_OpenIGTLinkIO)
    find_package(OpenIGTLinkIO REQUIRED)
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
    include_directories(${EIGEN_INCLUDE_DIR})
endmacro()

