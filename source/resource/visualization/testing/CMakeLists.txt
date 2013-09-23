
if (BUILD_TESTING)
	include_directories(${CustusX3_SOURCE_DIR}/source/resource/testUtilities)
	QT4_WRAP_CPP(RESOURCE_VISUALIZATION_TEST_SOURCE_FILES
        sscViewsWindow.h
        sscContainerWindow.h)
  
	set(RESOURCE_VISUALIZATION_TEST_SOURCE_FILES
        sscViewsWindow.cpp 
        sscContainerWindow.cpp
        sscTestVisualRendering.cpp
        sscTestViewContainer.cpp
        sscTestGPURayCaster.cpp
        sscTestVideoGraphics.h
        sscTestVideoGraphics.cpp
		${RESOURCE_VISUALIZATION_TEST_SOURCE_FILES}
		)
 
	include(cxTestMacros)
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestResourceVisualization cxCppUnitMain.cpp "${RESOURCE_VISUALIZATION_TEST_SOURCE_FILES}" cxResourceVisualization)
	
	#Catch
#        set(RESOURCE_TEST_CATCH_SOURCE_FILES
#            )
#        cx_catch_add_lib_and_exe(cxResource "${RESOURCE_TEST_CATCH_SOURCE_FILES}")
    
endif (BUILD_TESTING)
