
if(BUILD_TESTING)
    set(CX_TEST_CATCH_ORG_CUSTUSX_CORE_TRACKING_SHAPE_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_CORE_TRACKING_SHAPE_SOURCE_FILES
        ${CX_TEST_CATCH_ORG_CUSTUSX_CORE_TRACKING_SHAPE_MOC_SOURCE_FILES}
        cxtestTrackingShapePlugin.cpp
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_CORE_TRACKING_SHAPE_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_CORE_TRACKING_SHAPE_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_core_tracking_shape ${CX_TEST_CATCH_ORG_CUSTUSX_CORE_TRACKING_SHAPE_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_CORE_TRACKING_SHAPE_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_core_tracking_shape)
    target_include_directories(cxtest_org_custusx_core_tracking_shape
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_core_tracking_shape
      PRIVATE
      org_custusx_core_tracking_shape
      cxCatch
      cxtestUtilities
      cxResource)
    cx_add_tests_to_catch(cxtest_org_custusx_core_tracking_shape)

endif(BUILD_TESTING)
