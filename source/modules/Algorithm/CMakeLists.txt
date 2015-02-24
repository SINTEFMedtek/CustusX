###########################################################
##               Algorithm Plugin
##
##               Plugin
##
##  Description: A collection of all algorithms and associated widgets not in
##               other plugins. 
##
##
##  Dependencies: resource layer, service layer
##                 
##
###########################################################

set( MOC_HEADER_FILES
    gui/cxAlgorithmPlugin.h
    gui/cxFilterWidget.h
    gui/cxAllFiltersWidget.h
)

set( SOURCE_FILES
    ${MOC_HEADER_FILES}
    gui/cxAlgorithmPlugin.h
    gui/cxAlgorithmPlugin.cpp
    gui/cxFilterWidget.h
    gui/cxFilterWidget.cpp
    gui/cxAllFiltersWidget.h
    gui/cxAllFiltersWidget.cpp
)

qt5_wrap_cpp( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

cx_create_export_header("cxPluginAlgorithm")

add_library( cxPluginAlgorithm ${SOURCE_FILES} ${MOC_HEADER_FILES})
target_include_directories(cxPluginAlgorithm
    PUBLIC
    .
    ./logic
    ./gui
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(cxPluginAlgorithm
    PRIVATE
    cxResource
	#cxGui
    cxGUIExtenderService
    cxResourceFilter
    cxResourceWidgets
    cxResourceVisualization
	cxLogicManager
    cxPluginUtilities
    ${ITK_LIBRARIES}

)

