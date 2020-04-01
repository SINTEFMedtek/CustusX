
if(BUILD_TESTING)

    set(CX_TEST_CATCH_org_custusx_core_filemanager_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_org_custusx_core_filemanager_SOURCE_FILES
        cxtestToolFiles.cpp
        cxtestTestToolMesh.h
        cxtestTestToolMesh.cpp
        cxtestDataReaderWriter.cpp
        )

    qt5_wrap_cpp(CX_TEST_CATCH_org_custusx_core_filemanager_MOC_SOURCE_FILES ${CX_TEST_CATCH_org_custusx_core_filemanager_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_core_filemanager ${CX_TEST_CATCH_org_custusx_core_filemanager_SOURCE_FILES} ${CX_TEST_CATCH_org_custusx_core_filemanager_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_core_filemanager)
    target_include_directories(cxtest_org_custusx_core_filemanager
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(
        cxtest_org_custusx_core_filemanager
        PRIVATE
        cxtestUtilities
        cxtestResource
        org_custusx_core_filemanager
        cxResource
        cxLogicManager
        cxCatch
        cxGui
        CTKDICOMCore
    )
    cx_add_tests_to_catch(cxtest_org_custusx_core_filemanager)
        
endif(BUILD_TESTING)
