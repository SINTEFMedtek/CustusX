
if (BUILD_TESTING)
	include_directories(${CustusX3_SOURCE_DIR}/source/resource/testUtilities)

   # The following files contains test code not converted from cppunit to ctest.
   # Not working, but the code they test are still present in cx.
   #     sscContainerWindow.cpp
   #     sscTestViewContainer.cpp
   #     sscTestGPURayCaster.cpp
 
	set(CXTEST_SOURCES_TO_MOC
		sscViewsWindow.h
	)
	cx_add_class(RESOURCE_VISUALZATION_TEST_CATCH_SOURCE_FILES
		sscViewsWindow.cpp
		cxtestVideoGraphicsFixture
        cxtestVideoGraphics.cpp
		cxtestVisualRendering.cpp
		)
  
    cx_catch_add_lib_and_exe(cxResourceVisualization "${RESOURCE_VISUALZATION_TEST_CATCH_SOURCE_FILES}" "${CXTEST_SOURCES_TO_MOC}")
    
endif (BUILD_TESTING)
