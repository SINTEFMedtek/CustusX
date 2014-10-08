###########################################################
##               Resource plugin utilities
###########################################################

SET( CX_QT_MOC_HEADER_FILES
)

SET (cxPluginUtilities_SOURCE
    cxServiceTrackerCustomizer.h
    cxServiceTrackerListener.h
    cxRegisteredService.h
    cxRegisteredService.cpp
)

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})

#QT5_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

cx_create_export_header("cxResourcePluginUtilities")
ADD_LIBRARY ( cxPluginUtilities ${cxPluginUtilities_SOURCE} ${MOC_HEADER_FILES} )

target_link_libraries(cxPluginUtilities
    cxResource
    )


