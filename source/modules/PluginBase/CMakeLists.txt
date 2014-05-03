###########################################################
##
###########################################################

#cx_initialize_QT()

cx_add_class_qt_moc(PLUGINBASE_FILES
    cxPluginBase
	)

cx_add_class(PLUGINBASE_FILES
#	cxPluginBase
    )


#set(PLUGINBASE_FILES
#    cxPluginBase.h#
#	)
#QT4_WRAP_CPP( PLUGINBASE_FILES ${PLUGINBASE_FILES} )
#set(PLUGINBASE_FILES
#    ${PLUGINBASE_FILES}
#    cxPluginBase.cpp
#	)

#include_directories( 
#    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
#)

set(PLUGINBASE_LIBRARIES
    ${QT_LIBRARIES}
    #cxResource
    )

add_library( cxPluginBase ${PLUGINBASE_FILES})
target_link_libraries(cxPluginBase ${PLUGINBASE_LIBRARIES} )
