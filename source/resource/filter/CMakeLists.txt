###########################################################
##               Resource filter library
###########################################################

cx_add_class(CX_RESOURCE_FILTER_FILES
	cxFilterGroup
)
cx_add_class_qt_moc(CX_RESOURCE_FILTER_FILES
    cxFilter
    cxFilterImpl
    cxFilterTimedAlgorithm
    cxPipeline
    
    filters/cxDummyFilter
    filters/cxDilationFilter
    filters/cxBinaryThresholdImageFilter
    filters/cxConnectedThresholdImageFilter
    filters/cxContourFilter
    filters/cxBinaryThinningImageFilter3DFilter
    filters/cxSmoothingImageFilter
    filters/cxResampleImageFilter
    filters/cxGenericScriptFilter
    filters/cxColorVariationFilter
)

cx_create_export_header("cxResourceFilter")

add_library(cxResourceFilter ${CX_RESOURCE_FILTER_FILES})
target_include_directories(cxResourceFilter
    PUBLIC
    .
	./filters
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/source/ThirdParty
)
target_link_libraries(cxResourceFilter
    PRIVATE
    cxResource
    cxResourceVisualization
    ${ITK_LIBRARIES}
)

cx_doc_define_user_docs("${CMAKE_CURRENT_SOURCE_DIR}/doc")

cx_add_non_source_file("doc/org.custusx.resource.filter.md")
cx_add_non_source_file("doc/org.custusx.resource.filter.script.md")

add_subdirectory(testing)

