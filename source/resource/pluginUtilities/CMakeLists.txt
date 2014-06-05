###########################################################
##               Resource plugin utilities
###########################################################

SET( CX_QT_MOC_HEADER_FILES
)

SET (cxPluginUtilities_SOURCE
    cxServiceTrackerCustomizer.h
    cxServiceTrackerListener.h
)

#include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
include_directories(
    #${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    #.
    #${CustusX3_SOURCE_DIR}/source/service/patient     
    #${CustusX3_SOURCE_DIR}/source/service/tracking     
    #${CustusX3_SOURCE_DIR}/source/service/visualization
    #${CustusX3_SOURCE_DIR}/source/service/video
    #${CustusX3_SOURCE_DIR}/source/service/state
    )


#QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

#ADD_LIBRARY ( cxPluginUtilities ${cxPluginUtilities_SOURCE} ${MOC_HEADER_FILES} )

#target_link_libraries(cxPluginUtilities
    #CTKPluginFramework
    #cxResource
    #cxPatientService
    #cxTrackingService
    #cxVisualizationService
    #cxVideoService
    #cxStateService
#    )


