
if(BUILD_TESTING)

    set(CX_TEST_CATCH_org_custusx_core_port_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_org_custusx_core_port_SOURCE_FILES
        cxtestToolFiles.cpp
        cxtestTestToolMesh.h
        cxtestTestToolMesh.cpp
        cxtestDataReaderWriter.cpp
        )

    qt5_wrap_cpp(CX_TEST_CATCH_org_custusx_core_port_MOC_SOURCE_FILES ${CX_TEST_CATCH_org_custusx_core_port_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_core_port ${CX_TEST_CATCH_org_custusx_core_port_SOURCE_FILES} ${CX_TEST_CATCH_org_custusx_core_port_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_core_port)
    target_include_directories(cxtest_org_custusx_core_port
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(
        cxtest_org_custusx_core_port
        PRIVATE
        cxtestUtilities
        org_custusx_core_port
        cxResource
        cxCatch
    )
    cx_add_tests_to_catch(cxtest_org_custusx_core_port)
        
endif(BUILD_TESTING)
