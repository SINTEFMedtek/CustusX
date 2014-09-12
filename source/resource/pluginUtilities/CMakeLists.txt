###########################################################
##               Resource plugin utilities
###########################################################

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
include_directories(
    #${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    #.
    #${CustusX_SOURCE_DIR}/source/service/patient     
    #${CustusX_SOURCE_DIR}/source/service/tracking     
    #${CustusX_SOURCE_DIR}/source/service/visualization
    #${CustusX_SOURCE_DIR}/source/service/video
    #${CustusX_SOURCE_DIR}/source/service/state
    )

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
#    Qt5::Xml
#    Qt5::Multimedia
#    Qt5::Sql
#    ${Boost_LIBRARIES}
    )
    
add_library (cxPluginUtilities ${SOURCE_FILES})
target_link_libraries(cxPluginUtilities ${LIBRARIES} )



