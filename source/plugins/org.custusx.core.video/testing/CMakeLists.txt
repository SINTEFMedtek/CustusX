
if(BUILD_TESTING)

    set(CX_TEST_CATCH_org_custusx_core_video_MOC_SOURCE_FILES
        cxtestTestVideoConnectionWidget.h
    )
    set(CX_TEST_CATCH_org_custusx_core_video_SOURCE_FILES
        cxtestVideoPlugin.cpp
        cxCatchVideo.cpp
        cxtestTestVideoConnectionWidget.cpp
        cxtestTestVideoConnectionWidget.h
        cxtestCatchStreamingWidgets.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_org_custusx_core_video_MOC_SOURCE_FILES ${CX_TEST_CATCH_org_custusx_core_video_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_core_video ${CX_TEST_CATCH_org_custusx_core_video_SOURCE_FILES} ${CX_TEST_CATCH_org_custusx_core_video_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_core_video)
    target_include_directories(cxtest_org_custusx_core_video
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_core_video
        PRIVATE
        Qt5::Test
        org_custusx_core_video
        cxGui
        cxResourceWidgets
        cxResourceVisualization
        cxLogicManager
        cxtestUtilities
        cxGrabber
        cxCatch
    )
    cx_add_tests_to_catch(cxtest_org_custusx_core_video)

endif(BUILD_TESTING)
