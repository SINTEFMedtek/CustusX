###########################################################
##               Resource filter library
###########################################################

cx_add_class_qt_moc(CX_RESOURCE_FILTER_FILES
    cxFilter
    cxFilterImpl
    cxFilterTimedAlgorithm
    cxPipeline
)

cx_add_class(CX_RESOURCE_FILTER_FILES
)

cx_create_export_header("cxResourceFilter")
add_library( cxResourceFilter ${CX_RESOURCE_FILTER_FILES})
target_include_directories(cxResourceFilter
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/source/ThirdParty
)
target_link_libraries(cxResourceFilter
    PRIVATE
    cxResource
)
