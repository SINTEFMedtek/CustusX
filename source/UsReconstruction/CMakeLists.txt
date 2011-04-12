# =========================================================
# sscUsReconstruction
# =========================================================

cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

SET( MOC_HEADER_FILES
   sscReconstructer.h
   sscReconstructionWidget.h
  sscReconstructOutputValueParamsInterfaces.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
  matrixInterpolation.cpp
  sscReconstructer.cpp
  sscReconstructionWidget.cpp
  sscReconstructOutputValueParamsInterfaces.cpp
  sscReconstructOutputValueParamsInterfaces.h
  sscReconstructedOutputVolumeParams.h
  sscReconstructAlgorithm.h
  sscThunderVNNReconstructAlgorithm.h
  sscThunderVNNReconstructAlgorithm.cpp
  sscPNNReconstructAlgorithm.h
  sscPNNReconstructAlgorithm.cpp
  utils/sscReconstructHelper.h
  utils/sscReconstructHelper.cpp
  #../../modules/ultrasoundAcquisition/source/gui/probeXmlConfigParser.h
  #../../modules/ultrasoundAcquisition/source/gui/probeXmlConfigParser.cpp
  sscUSFrameData.h
  sscUSFrameData.cpp
  cxUsReconstructionFileMaker.h
  cxUsReconstructionFileMaker.cpp
  cxUsReconstructionFileReader.h
  cxUsReconstructionFileReader.cpp
  )

# =========================================================

SET( sscUsReconstructionLib_LIB_FILES
    SSC
    cxCommon 
    dataManager
    #thunder 
    toolManager
    ITKCommon
    )

# 
FIND_LIBRARY(OPENCL_LIBRARIES OpenCL)
IF( OPENCL_LIBRARIES )
    option(USE_US_RECONSTRUCTION_THUNDER "use us reconstruction library Thunder" ON)
ELSEIF( OPENCL_LIBRARIES )
    option(USE_US_RECONSTRUCTION_THUNDER "use us reconstruction library Thunder" OFF)
ENDIF( OPENCL_LIBRARIES )
            
if (USE_US_RECONSTRUCTION_THUNDER)
    ADD_SUBDIRECTORY(Thunder)
    include_directories( Thunder )
    link_directories( ${CMAKE_CURRENT_BINARY_DIR}/Thunder )
    set( sscUsReconstructionLib_LIB_FILES thunder ${sscUsReconstructionLib_LIB_FILES} )
    set(THUNDER_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Thunder)
endif (USE_US_RECONSTRUCTION_THUNDER)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/recConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/recConfig.h" )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
#include_directories( Thunder
#  ${CMAKE_CURRENT_BINARY_DIR}
#)
#link_directories( ${CMAKE_CURRENT_BINARY_DIR}/Thunder )

# =========================================================
# create the target
add_library( sscUsReconstructionLib ${SOURCE_FILES} )
target_link_libraries( sscUsReconstructionLib ${sscUsReconstructionLib_LIB_FILES} )
# =========================================================

ADD_SUBDIRECTORY( testing )

