
if (BUILD_TESTING)
	include_directories(${CustusX3_SOURCE_DIR}/source/resource/testUtilities)

    set(CXTEST_SOURCES_TO_MOC
        sscViewsWindow.h
    )
  
    cx_add_class(RESOURCE_VISUALZATION_TEST_CATCH_SOURCE_FILES
        sscTestVisualRendering.cpp
        sscViewsWindow.cpp
    )
    cx_catch_add_lib_and_exe(cxResourceVisualization "${RESOURCE_VISUALZATION_TEST_CATCH_SOURCE_FILES}" "${CXTEST_SOURCES_TO_MOC}")
    
endif (BUILD_TESTING)
