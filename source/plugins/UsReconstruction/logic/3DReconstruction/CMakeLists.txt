###########################################################
##               3D US Reconstruction
##
##
###########################################################

cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms    
    ${CustusX3_SOURCE_DIR}/source/plugins/USReconstruction/logic/3DReconstruction

    )

SET( MOC_HEADER_FILES
   sscReconstructer.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
  sscReconstructer.cpp
  sscReconstructer.h
  sscReconstructedOutputVolumeParams.h
  sscReconstructAlgorithm.h
  sscThunderVNNReconstructAlgorithm.h
  sscThunderVNNReconstructAlgorithm.cpp
  sscPNNReconstructAlgorithm.h
  sscPNNReconstructAlgorithm.cpp
  utils/sscReconstructHelper.h
  utils/sscReconstructHelper.cpp
  sscReconstructCore.cpp
  sscReconstructCore.h
      )

# =========================================================

SET( cxPluginUSReconstructionLogic_LIB_FILES
    SSC
#    cxUtilities 
#    cxSettings
#    cxUtilities
#    cxPatientService
    #thunder 
#    cxTrackingService
#    ITKCommon
    )


include_directories(
    .
    )



# openCL
#----------
if (SSC_USE_OpenCL)

    find_package( OpenCL REQUIRED)
else ()
    find_package( OpenCL QUIET)
endif ()

IF( OpenCL_FOUND )
    option(SSC_USE_OpenCL "Use OpenCL" ON)
ELSE()
    option(SSC_USE_OpenCL "Use OpenCL" OFF)
ENDIF()

#message(STATUS OPENCL_FOUND: ${OPENCL_FOUND})
#message(STATUS OPENCL_INCLUDE_DIRS: ${OPENCL_INCLUDE_DIRS})
#message(STATUS OPENCL_LIBRARIES: ${OPENCL_LIBRARIES})
get_filename_component(OPENCL_LIBRARY_DIRS ${OPENCL_LIBRARIES} PATH)
#message(STATUS OPENCL_LIBRARY_DIRS: ${OPENCL_LIBRARY_DIRS})

# 
#FIND_LIBRARY(OPENCL_LIBRARIES OpenCL)
#IF( OPENCL_LIBRARIES )
#    option(USE_US_RECONSTRUCTION_THUNDER "use us reconstruction library Thunder" ON)
#ELSE( OPENCL_LIBRARIES )
#    option(USE_US_RECONSTRUCTION_THUNDER "use us reconstruction library Thunder" OFF)
#ENDIF( OPENCL_LIBRARIES )

# Thunder requires OpenCL - make it optional            
if (SSC_USE_OpenCL)
    include_directories( Thunder
        ${OPENCL_INCLUDE_DIRS}
    )
    SET( SOURCE_FILES
      ${SOURCE_FILES}    
      Thunder/holger_time.h
      Thunder/holger_time.cpp
      Thunder/setup.h
      Thunder/setup.cpp
      Thunder/utils.h
      Thunder/utils.cpp
      Thunder/reconstruct_vnn.h
      Thunder/reconstruct_vnn.cpp
      Thunder/kernels.ocl    
      )
    set(THUNDER_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Thunder)
    
#    IF (APPLE) # mac
#      FIND_LIBRARY(OPENCL_LIBRARIES OpenCL DOC "OpenCL lib for OSX")
#      FIND_PATH(OPENCL_INCLUDE_DIR OpenCL/cl.h DOC "Include for OpenCL on OSX")
#    ELSEIF (WIN32) # windows
#      FIND_PATH(OPENCL_INCLUDE_DIR CL/cl.h )
#      FIND_LIBRARY(OPENCL_LIBRARIES opencl64 )
#      IF( OPENCL_LIBRARIES )
#          FIND_LIBRARY(OPENCL_LIBRARIES opencl32 )
#      ENDIF( OPENCL_LIBRARIES )
#      INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIR} )
#    ELSE (APPLE) # i.e. linux and similar
#    	FIND_PATH(OPENCL_INCLUDE_DIR CL/cl.h )
#    	FIND_LIBRARY(OPENCL_LIBRARIES OpenCL )
#    ENDIF (APPLE)    
endif (SSC_USE_OpenCL)

# create the target
add_library( sscUS3DReconstruction ${SOURCE_FILES} )

# Don't try to use OpenCl if we haven't got it
IF( SSC_USE_OpenCL )
    target_link_libraries( sscUS3DReconstruction SSC cxUtilities ${OPENCL_LIBRARIES})
ELSE()
    target_link_libraries( sscUS3DReconstruction SSC cxUtilities)
ENDIF()


configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/recConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/recConfig.h" )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

# =========================================================
# =========================================================
