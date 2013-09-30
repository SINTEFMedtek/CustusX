
if (BUILD_TESTING)
	include_directories(${CustusX3_SOURCE_DIR}/source/resource/testUtilities)

	QT4_WRAP_CPP(RESOURCE_VISUALIZATION_TEST_SOURCE_FILES
#        sscViewsWindow.h
        sscContainerWindow.h
    )
  
  #CAS: TODO
	set(RESOURCE_VISUALIZATION_TEST_SOURCE_FILES
        sscContainerWindow.cpp
        sscTestViewContainer.cpp
        sscTestGPURayCaster.cpp
 #       sscTestVideoGraphics.h
 #       sscTestVideoGraphics.cpp
		${RESOURCE_VISUALIZATION_TEST_SOURCE_FILES}
		)
 
    cx_add_class_qt_moc(RESOURCE_VISUALZATION_TEST_CATCH_SOURCE_FILES
        sscViewsWindow
    )
    cx_add_class(RESOURCE_VISUALZATION_TEST_CATCH_SOURCE_FILES
		cxtestVideoGraphicsFixture
        cxtestVideoGraphics.cpp
		cxtestVisualRendering.cpp
    )
    cx_catch_add_lib_and_exe(cxResourceVisualization "${RESOURCE_VISUALZATION_TEST_CATCH_SOURCE_FILES}")
    
endif (BUILD_TESTING)
