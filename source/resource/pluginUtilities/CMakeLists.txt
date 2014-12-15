###########################################################
##               Resource plugin utilities
###########################################################

cx_create_export_header("cxResourcePluginUtilities")

cx_add_class_qt_moc(SOURCE_FILES
)
cx_add_class(SOURCE_FILES
    cxServiceTrackerCustomizer.h
    cxServiceTrackerListener.h
    cxRegisteredService
)

set(LIBRARIES
    ${SSC_GCOV_LIBRARY}
    Qt5::Widgets
    CTKPluginFramework
)

add_library(cxPluginUtilities ${SOURCE_FILES})
target_include_directories(cxPluginUtilities
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(cxPluginUtilities PRIVATE ${LIBRARIES} )
