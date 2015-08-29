
if(BUILD_TESTING)
    set(CX_TEST_CATCH_ORG_CUSTUSX_ROBOT_UR5_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_ROBOT_UR5_SOURCE_FILES
        ${CX_TEST_CATCH_ORG_CUSTUSX_ROBOT_UR5_MOC_SOURCE_FILES}
        cxtestUr5Plugin.cpp
        cxtestUr5TestFixture.h
        cxtestUr5TestFixture.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_ROBOT_UR5_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_ROBOT_UR5_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_robot_ur5 ${CX_TEST_CATCH_ORG_CUSTUSX_ROBOT_UR5_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_ROBOT_UR5_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_robot_ur5)
    target_include_directories(cxtest_org_custusx_robot_ur5
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_robot_ur5 PRIVATE org_custusx_robot_ur5 cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_robot_ur5)

endif(BUILD_TESTING)
