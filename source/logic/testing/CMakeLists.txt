
if(BUILD_TESTING)

    set(CX_TEST_CATCH_LOGIC_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_LOGIC_SOURCE_FILES
        cxtestPlugins.cpp
        cxtestStreamerService.cpp
        cxtestDummyLogicManager.h
        cxtestDummyLogicManager.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_LOGIC_MOC_SOURCE_FILES ${CX_TEST_CATCH_LOGIC_MOC_SOURCE_FILES})
    add_library(cxtestLogicManager ${CX_TEST_CATCH_LOGIC_SOURCE_FILES} ${CX_TEST_CATCH_LOGIC_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtestLogicManager)
    target_include_directories(cxtestLogicManager
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtestLogicManager PRIVATE cxLogicManager cxCatch)
    cx_add_tests_to_catch(cxtestLogicManager)

endif(BUILD_TESTING)
