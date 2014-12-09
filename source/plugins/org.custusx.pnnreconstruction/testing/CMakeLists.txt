
if(BUILD_TESTING)
    set(CX_TEST_CATCH_ORG_CUSTUSX_PNNRECONSTRUCTION_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_PNNRECONSTRUCTION_SOURCE_FILES
        cxtestPNNPlugin.cpp
        cxtestDummyPNNReconstruction.h
        cxtestDummyPNNReconstruction.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_PNNRECONSTRUCTION_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_PNNRECONSTRUCTION_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_pnnreconstruction ${CX_TEST_CATCH_ORG_CUSTUSX_PNNRECONSTRUCTION_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_PNNRECONSTRUCTION_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_pnnreconstruction)
    target_include_directories(cxtest_org_custusx_pnnreconstruction
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_pnnreconstruction PRIVATE org_custusx_pnnreconstruction cxtest_org_custusx_usreconstruction cxtestUtilities cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_pnnreconstruction)

endif(BUILD_TESTING)
