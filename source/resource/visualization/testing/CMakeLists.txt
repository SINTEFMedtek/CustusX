
if (BUILD_TESTING)
	include_directories(
	    ${CustusX_SOURCE_DIR}/source/resource/testUtilities
	    ${CustusX_BINARY_DIR}/source/resource/testUtilities
	    ${CustusX_SOURCE_DIR}/source/ThirdParty/catch 
	    )

   # The following files contains test code not converted from cppunit to ctest.
   # Not working, but the code they test are still present in cx.
   #     cxTestGPURayCaster.cpp
#        cx_catch__private_define_platform_specific_linker_options()
        #cx_add_class(RESOURCE_VISUALZATION_TEST_UTILITIES_SOURCE_FILES
        #	cxtestRenderTester
#		cxTestRenderSpeed
#		cxViewsFixture
#5		)
#        add_library(cxResourceVisualizationTestUtilities ${CX_CATCH_SHARED_LIB_TYPE} ${RESOURCE_VISUALZATION_TEST_UTILITIES_SOURCE_FILES})
#        target_link_libraries(cxResourceVisualizationTestUtilities cxResourceVisualization cxtestUtilities)

        #cx_create_export_header("cxResourceVisualizationTestUtilities")

        #cx_add_test_library(cxResourceVisualizationTestUtilities ${RESOURCE_VISUALZATION_TEST_UTILITIES_SOURCE_FILES})
        #target_link_libraries(cxResourceVisualizationTestUtilities cxResourceVisualization cxtestUtilities)


	set(CXTEST_SOURCES_TO_MOC
		cxViewsWindow.h
		cxViewsFixture.h
	)

        set(CXTEST_SOURCES
                cxtestRenderTester.h
                cxtestRenderTester.cpp
                cxTestRenderSpeed.h
                cxTestRenderSpeed.cpp
                cxViewsFixture.cpp
                cxViewsWindow.cpp
                cxtestVideoGraphicsFixture.cpp
                cxtestVideoGraphics.cpp
                cxtestVisualRendering.cpp
                cxtestImageEnveloper.cpp
                cxtestVisualizationService.cpp
        )
#	cx_add_class(RESOURCE_VISUALZATION_TEST_CATCH_SOURCE_FILES
#		cxViewsFixture.cpp
#		cxViewsWindow.cpp
#		cxtestVideoGraphicsFixture
#        cxtestVideoGraphics.cpp
#		cxtestVisualRendering.cpp
#        cxtestImageEnveloper.cpp
#		)
  
        #cx_catch_add_lib_and_exe(cxResourceVisualization
        #        "${CXTEST_SOURCES}"
        #	"${CXTEST_SOURCES_TO_MOC}"
        #	"cxResourceVisualizationTestUtilities")

        cx_catch_add_lib_and_exe(""
                "${CXTEST_SOURCES}"
                "${CXTEST_SOURCES_TO_MOC}")
    
endif (BUILD_TESTING)
