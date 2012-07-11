###########################################################
##               US Reconstruction Plugin Logic
##
##               Logic Part
##
###########################################################


cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

SET( MOC_HEADER_FILES
   sscReconstructManager.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
  sscReconstructManager.cpp
  sscReconstructManager.h
  )

# =========================================================

SET( cxPluginUSReconstructionLogic_LIB_FILES
    SSC
    sscUS3DReconstruction
    cxUtilities 
    cxSettings
    cxAlgorithms_Res
    cxPatientService
    cxTrackingService
    )


include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms
    )

# =========================================================
# create the target
add_library( cxPluginUSReconstructionLogic ${SOURCE_FILES} )
target_link_libraries( cxPluginUSReconstructionLogic ${cxPluginUSReconstructionLogic_LIB_FILES} )

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginUSReconstructionLogic)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginUSReconstructionLogic_unity 
        		${cxPluginUSReconstructionLogic_LIB_FILES}
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
# =========================================================

