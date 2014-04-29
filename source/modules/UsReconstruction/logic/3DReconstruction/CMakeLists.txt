###########################################################
##               3D US Reconstruction
##
##
###########################################################

cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
include_directories(
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ${CustusX3_SOURCE_DIR}/source/modules/USReconstruction/logic/3DReconstruction
    )

SET( MOC_HEADER_FILES
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )


SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
  cxReconstructedOutputVolumeParams.h
  cxReconstructedOutputVolumeParams.cpp
  cxReconstructAlgorithm.h
  cxPNNReconstructAlgorithm.h
  cxPNNReconstructAlgorithm.cpp
  cxReconstructCore.cpp
  cxReconstructCore.h
  cxReconstructPreprocessor.cpp
  cxReconstructPreprocessor.h
	TordReconstruct/cxSyntheticVolume.h
	TordReconstruct/cxSyntheticVolume.cpp
	TordReconstruct/cxSimpleSyntheticVolume.h
      )

# =========================================================

include_directories(
    .
    )

if (CX_USE_OPENCL_UTILITY)
	include_directories(
		TordReconstruct
		${OPENCL_INCLUDE_DIRS}
		)
	SET( SOURCE_FILES
		${SOURCE_FILES}
		TordReconstruct/TordTest.h
		TordReconstruct/TordTest.cpp
		TordReconstruct/cxTordAlgorithm.h
		TordReconstruct/cxTordAlgorithm.cpp
		TordReconstruct/kernels.cl
		)
   SET(TORD_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/TordReconstruct)

endif (CX_USE_OPENCL_UTILITY)

# create the target
add_library( sscUS3DReconstruction ${SOURCE_FILES} )

set(sscUS3DReconstruction_LIBRARIES
    cxResource
    cxResourceUsReconstructionTypes
	cxLogicManager
    )
    
if( CX_USE_OPENCL_UTILITY )
    set(sscUS3DReconstruction_LIBRARIES
        ${sscUS3DReconstruction_LIBRARIES}
        ${OPENCL_LIBRARIES})
endif()

target_link_libraries( sscUS3DReconstruction ${sscUS3DReconstruction_LIBRARIES})

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/recConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/recConfig.h"
  )
  
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

# =========================================================
# =========================================================
