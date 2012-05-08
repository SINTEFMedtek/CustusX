###########################################################
##               Template Plugin Logic
##
##               Logic Part
##
###########################################################


SET( MOC_HEADER_FILES
    cxCenterline.h
    cxResample.h
    cxBinaryThresholdImageFilter.h
    cxContour.h
    cxConnectedThresholdImageFilter.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
    cxTrackingDataToVolume.h
    cxTrackingDataToVolume.cpp
    cxCenterline.h
    cxCenterline.cpp
    cxResample.h
    cxResample.cpp
    cxBinaryThresholdImageFilter.h
    cxBinaryThresholdImageFilter.cpp
    cxContour.h
    cxContour.cpp
    cxConnectedThresholdImageFilter.h
    cxConnectedThresholdImageFilter.cpp  
  )

# =========================================================


include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms
    
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking

#    ${CustusX3_SOURCE_DIR}/source/logic/stateMachineManager
#    ${CustusX3_SOURCE_DIR}/source/UsReconstruction
#    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic
#    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/logic    
    )

# =========================================================
# create the target
add_library( cxPluginAlgorithmLogic ${SOURCE_FILES} )
target_link_libraries( cxPluginAlgorithmLogic cxPatientService cxTrackingService cxAlgorithms_Res )

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginAlgorithmLogic)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginAlgorithmLogic_unity 
        		cxPatientService 
        		cxTrackingService 
        		cxAlgorithms_Res
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
# =========================================================