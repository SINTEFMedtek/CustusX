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
)

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
include_directories(
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ${CustusX_SOURCE_DIR}/source/service/tracking     
    ${CustusX_SOURCE_DIR}/source/service/visualization
    ${CustusX_SOURCE_DIR}/source/service/video
    ${CustusX_SOURCE_DIR}/source/service/state

    ${CustusX_BINARY_DIR}/source/service/tracking     
    ${CustusX_BINARY_DIR}/source/service/visualization
    ${CustusX_BINARY_DIR}/source/service/video
    ${CustusX_BINARY_DIR}/source/service/state
    )


cx_remove_duplicate_include_directories()
QT5_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

cx_create_export_header("cxLogicManager")

add_library( cxLogicManager ${cxLogicManager_SOURCE} ${MOC_HEADER_FILES} )

target_link_libraries(cxLogicManager
	CTKPluginFramework
    cxResource
    cxVisualizationService
	cxStateService
    )

add_subdirectory(testing)

