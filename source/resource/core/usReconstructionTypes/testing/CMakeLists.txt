
if(BUILD_TESTING)
    set(CXTEST_SOURCES_TO_MOC
    )

    set(RESOURCE_USRECONSTRUCTIONTYPES_TEST_CATCH_SOURCE_FILES
        cxtestUSReconstructionFileFixture.h
        cxtestUSReconstructionFileFixture.cpp
        cxtestCatchUSReconstructionFile.cpp
        cxtestUSReconstructInputDataAlgorithms.cpp
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtestResourceUsReconstructionTypes ${RESOURCE_USRECONSTRUCTIONTYPES_TEST_CATCH_SOURCE_FILES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtestResourceUsReconstructionTypes)
    target_include_directories(cxtestResourceUsReconstructionTypes
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtestResourceUsReconstructionTypes PRIVATE cxtestResource cxLogicManager cxResource cxCatch)
    cx_add_tests_to_catch(cxtestResourceUsReconstructionTypes)
endif()
