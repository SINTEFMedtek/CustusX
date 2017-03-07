###########################################################
##               Logic Layer
###########################################################

set(CX_QT_MOC_HEADER_FILES
    cxLogicManager.h
    cxPluginFramework.h
)

set(cxLogicManager_SOURCE
    cxLogicManager.h
    cxLogicManager.cpp
    cxPluginFramework.h
    cxPluginFramework.cpp
    cxPluginFrameworkUtilities.h
    cxPluginFrameworkUtilities.cpp
)

qt5_wrap_cpp( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

cx_create_export_header("cxLogicManager")

add_library( cxLogicManager ${cxLogicManager_SOURCE} ${MOC_HEADER_FILES} )
target_include_directories(cxLogicManager
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(cxLogicManager
    PUBLIC
    cxResource
    cxResourceVisualization

    PRIVATE
    Qt5::Core
    cxPluginUtilities
)

add_subdirectory(testing)

