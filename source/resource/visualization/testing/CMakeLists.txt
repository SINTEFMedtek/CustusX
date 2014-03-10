
if (BUILD_TESTING)
	include_directories(${CustusX3_SOURCE_DIR}/source/resource/testUtilities)

   # The following files contains test code not converted from cppunit to ctest.
   # Not working, but the code they test are still present in cx.
   #     sscContainerWindow.cpp
   #     sscTestViewContainer.cpp
   #     sscTestGPURayCaster.cpp
#        cx_catch__private_define_platform_specific_linker_options()
        cx_add_class(RESOURCE_VISUALZATION_TEST_UTILITIES_SOURCE_FILES
		cxtestRenderTester
		cxTestRenderSpeed
#		cxViewsFixture
		)
#        add_library(cxResourceVisualizationTestUtilities ${CX_CATCH_SHARED_LIB_TYPE} ${RESOURCE_VISUALZATION_TEST_UTILITIES_SOURCE_FILES})
#        target_link_libraries(cxResourceVisualizationTestUtilities cxResourceVisualization cxtestUtilities)

        cx_add_test_library(cxResourceVisualizationTestUtilities ${RESOURCE_VISUALZATION_TEST_UTILITIES_SOURCE_FILES})
        target_link_libraries(cxResourceVisualizationTestUtilities cxResourceVisualization cxtestUtilities)


	set(CXTEST_SOURCES_TO_MOC
		sscViewsWindow.h
		cxViewsFixture.h
	)
	cx_add_class(RESOURCE_VISUALZATION_TEST_CATCH_SOURCE_FILES
		cxViewsFixture.cpp
		sscViewsWindow.cpp
		cxtestVideoGraphicsFixture
        cxtestVideoGraphics.cpp
		cxtestVisualRendering.cpp
                cxtestImageEnveloper.cpp
		)
  
	cx_catch_add_lib_and_exe(cxResourceVisualization
		"${RESOURCE_VISUALZATION_TEST_CATCH_SOURCE_FILES}"
		"${CXTEST_SOURCES_TO_MOC}"
		"cxResourceVisualizationTestUtilities")
    
endif (BUILD_TESTING)
