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
    logic/cxDummyFilter.h
    logic/cxBinaryThresholdImageFilter.h
    logic/cxConnectedThresholdImageFilter.h
    logic/cxContourFilter.h
    logic/cxBinaryThinningImageFilter3DFilter.h
    logic/cxSmoothingImageFilter.h
    logic/cxResampleImageFilter.h
    logic/cxDilationFilter.h
    gui/cxAlgorithmPlugin.h
    gui/cxFilterWidget.h
    gui/cxPipelineWidget.h
    gui/cxAllFiltersWidget.h
    gui/cxCompactFilterSetupWidget.h
)

set( SOURCE_FILES
    ${MOC_HEADER_FILES}
    logic/cxDummyFilter.h
    logic/cxDummyFilter.cpp
    logic/cxDilationFilter.h
    logic/cxDilationFilter.cpp
    logic/cxTrackingDataToVolume.h
    logic/cxTrackingDataToVolume.cpp
    logic/cxBinaryThresholdImageFilter.h
    logic/cxBinaryThresholdImageFilter.cpp
    logic/cxConnectedThresholdImageFilter.h
    logic/cxConnectedThresholdImageFilter.cpp
    logic/cxContourFilter.h
    logic/cxContourFilter.cpp
    logic/cxBinaryThinningImageFilter3DFilter.h
    logic/cxBinaryThinningImageFilter3DFilter.cpp
    logic/cxSmoothingImageFilter.h
    logic/cxSmoothingImageFilter.cpp
    logic/cxResampleImageFilter.h
    logic/cxResampleImageFilter.cpp
    gui/cxAlgorithmPlugin.h
    gui/cxAlgorithmPlugin.cpp
    gui/cxFilterWidget.h
    gui/cxFilterWidget.cpp
    gui/cxPipelineWidget.h
    gui/cxPipelineWidget.cpp
    gui/cxAllFiltersWidget.h
    gui/cxAllFiltersWidget.cpp
    gui/cxCompactFilterSetupWidget.h
    gui/cxCompactFilterSetupWidget.cpp
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

add_subdirectory(testing)
