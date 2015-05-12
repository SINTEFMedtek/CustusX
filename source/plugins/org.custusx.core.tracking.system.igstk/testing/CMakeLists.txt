
if(BUILD_TESTING)

    set(CX_TEST_CATCH_org_custusx_core_tracking_igstk_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_org_custusx_core_tracking_igstk_SOURCE_FILES
        cxtestToolFiles.cpp
        cxExportHack.h
        cxExportHack.cpp
        )

    qt5_wrap_cpp(CX_TEST_CATCH_org_custusx_core_tracking_igstk_MOC_SOURCE_FILES ${CX_TEST_CATCH_org_custusx_core_tracking_igstk_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_core_tracking_igstk ${CX_TEST_CATCH_org_custusx_core_tracking_igstk_SOURCE_FILES} ${CX_TEST_CATCH_org_custusx_core_tracking_igstk_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_core_tracking_igstk)
    target_include_directories(cxtest_org_custusx_core_tracking_igstk
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_core_tracking_igstk PRIVATE org_custusx_core_tracking_system_igstk cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_core_tracking_igstk)
        
endif(BUILD_TESTING)
