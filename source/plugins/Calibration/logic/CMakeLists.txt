###########################################################
##               Calibration Plugin Logic
##
##               Logic Part
##
###########################################################



SET( MOC_HEADER_FILES
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
  cxTemporalCalibration.h
  cxTemporalCalibration.cpp
  )

# =========================================================


include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    )

# =========================================================
# create the target
add_library( cxPluginCalibrationLogic ${SOURCE_FILES} )
target_link_libraries( cxPluginCalibrationLogic
    cxUtilities 
    cxSettings 
    cxPluginAcquisitionLogic
      )
      
#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginCalibrationLogic)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginCalibrationLogic_unity 
        		cxUtilities 
                cxSettings 
                cxPluginAcquisitionLogic
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
# =========================================================

#ADD_SUBDIRECTORY( testing )