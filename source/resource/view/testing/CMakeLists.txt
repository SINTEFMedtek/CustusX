
if(BUILD_TESTING)

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
        cxtestStream2DRep3D.cpp
        cxtestSharedOpenGLContext.cpp
        cxtestRenderWindowFactory.cpp
        cxtestViewServiceMockWithRenderWindowFactory.h
        cxtestViewServiceMockWithRenderWindowFactory.cpp
        cxtestGeometricRep.cpp
        cxtestMultiViewCache.cpp
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtestResourceVisualization ${CXTEST_SOURCES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtestResourceVisualization)
    target_include_directories(cxtestResourceVisualization
        PUBLIC
        .
        ..
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CustusX_SOURCE_DIR}/source/resource/core/testing
        ${CustusX_SOURCE_DIR}/source/resource/testUtilities
    )
    target_link_libraries(cxtestResourceVisualization
        PRIVATE
        cxtestUtilities
        cxtestResource
        cxResource
        cxResourceVisualization
        cxCatch
        cxLogicManager
    )
    cx_add_tests_to_catch(cxtestResourceVisualization)
    
endif(BUILD_TESTING)
