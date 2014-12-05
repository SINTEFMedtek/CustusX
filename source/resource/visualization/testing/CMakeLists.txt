
if (BUILD_TESTING)
	include_directories(
	    ${CustusX_SOURCE_DIR}/source/resource/testUtilities
	    ${CustusX_BINARY_DIR}/source/resource/testUtilities
	    ${CustusX_SOURCE_DIR}/source/ThirdParty/catch 
	    )

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
        )

        cx_catch_add_lib_and_exe(""
                "${CXTEST_SOURCES}"
                "${CXTEST_SOURCES_TO_MOC}")
    
endif (BUILD_TESTING)
