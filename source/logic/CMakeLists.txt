###########################################################
##               Logic Layer
###########################################################

# definition of the cxLogicManager module:

SET( CX_QT_MOC_HEADER_FILES
	cxServiceController.h
	cxLogicManager.h
	cxPluginFramework.h
)

SET (cxLogicManager_SOURCE
    cxServiceController.h
    cxServiceController.cpp
    cxLogicManager.h
    cxLogicManager.cpp
	cxLegacySingletons.h
	cxLegacySingletons.cpp
	cxPluginFramework.h
	cxPluginFramework.cpp
	cxPluginFrameworkUtilities.h
	cxPluginFrameworkUtilities.cpp
    cxServiceTrackerCustomizer.h
    cxServiceTrackerListener.h
)

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
include_directories(
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ${CustusX3_SOURCE_DIR}/source/service/patient     
    ${CustusX3_SOURCE_DIR}/source/service/tracking     
    ${CustusX3_SOURCE_DIR}/source/service/visualization
    ${CustusX3_SOURCE_DIR}/source/service/video
    ${CustusX3_SOURCE_DIR}/source/service/state
    )


QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxLogicManager ${cxLogicManager_SOURCE} ${MOC_HEADER_FILES} )

target_link_libraries(cxLogicManager
	CTKPluginFramework
    cxResource
    cxPatientService
    cxTrackingService
    cxVisualizationService
    cxVideoService
	cxStateService
    )


