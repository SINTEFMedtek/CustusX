###########################################################
##               Resource plugin utilities
###########################################################

cx_create_export_header("cxResourcePluginUtilities")

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})

set(SOURCE_FILES )
cx_add_class_qt_moc(SOURCE_FILES
	)
cx_add_class(SOURCE_FILES
	cxServiceTrackerCustomizer.h
    cxServiceTrackerListener.h
    cxRegisteredService
    )    


set (LIBRARIES
    ${SSC_GCOV_LIBRARY}
    Qt5::Widgets
    cxResource
    CTKPluginFramework
    )
    
add_library (cxPluginUtilities ${SOURCE_FILES})
target_link_libraries(cxPluginUtilities ${LIBRARIES} )



