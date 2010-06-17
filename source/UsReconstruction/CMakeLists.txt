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
  sscReconstructedOutputVolumeParams.h
  sscReconstructAlgorithm.h
  sscThunderVNNReconstructAlgorithm.h
  sscThunderVNNReconstructAlgorithm.cpp
  #../../modules/ultrasoundAcquisition/source/gui/probeXmlConfigParser.h
  #../../modules/ultrasoundAcquisition/source/gui/probeXmlConfigParser.cpp
  probeXmlConfigParser.h
  probeXmlConfigParser.cpp
  )

# =========================================================

# 
set(THUNDER_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Thunder)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/recConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/recConfig.h" )

ADD_SUBDIRECTORY(Thunder)

include_directories( Thunder
  #${SSC_SOURCE_DIR}/Code/UsReconstruction/Thunder 
  ${CMAKE_CURRENT_BINARY_DIR}
)
link_directories( Thunder )

# =========================================================
# create the target
add_library( sscUsReconstructionLib ${SOURCE_FILES} )
target_link_libraries( sscUsReconstructionLib SSC thunder ITKCommon)
# =========================================================



