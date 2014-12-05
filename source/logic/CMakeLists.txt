###########################################################
##               Logic Layer
###########################################################

# definition of the cxLogicManager module:

SET( CX_QT_MOC_HEADER_FILES
	cxLogicManager.h
	cxPluginFramework.h
)

SET (cxLogicManager_SOURCE
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
	${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    .
    )


cx_remove_duplicate_include_directories()
QT5_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

cx_create_export_header("cxLogicManager")

add_library( cxLogicManager ${cxLogicManager_SOURCE} ${MOC_HEADER_FILES} )

target_link_libraries(cxLogicManager
	CTKPluginFramework
    cxResource
	cxResourceVisualization
    )

add_subdirectory(testing)

