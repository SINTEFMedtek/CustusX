###########################################################
##               Resource filter library
###########################################################

cx_add_class_qt_moc(CX_RESOURCE_FILTER_FILES
	cxFilter
	cxFilterImpl
	cxFilterTimedAlgorithm
#	cxDummyFilter
	cxPipeline
#	cxBinaryThresholdImageFilter
##	cxConnectedThresholdImageFilter
#	cxContourFilter
#	cxBinaryThinningImageFilter3DFilter
#	cxSmoothingImageFilter
#	cxResampleImageFilter
#	cxDilationFilter
	)

cx_add_class(CX_RESOURCE_FILTER_FILES
	)

unset(CX_RESOURCE_FILTER_INCLUDE_DIRECTORIES)
set(CX_RESOURCE_FILTER_INCLUDE_DIRECTORIES
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    CACHE INTERNAL "Include directories for resource filter library")
include_directories(${CX_RESOURCE_FILTER_INCLUDE_DIRECTORIES} )

set(CX_RESOURCE_FILTER_LIBRARIES
    ${CX_RESOURCE_FILTER_LIBRARIES}
    cxResource
    )

cx_create_export_header("cxResourceFilter")
add_library( cxResourceFilter ${CX_RESOURCE_FILTER_FILES})
target_link_libraries(cxResourceFilter ${CX_RESOURCE_FILTER_LIBRARIES} )

#add_subdirectory(testing)
