
if(BUILD_TESTING)
    cx_initialize_cppunit()

    set(CXTEST_SOURCES_TO_MOC
    )

    set(CX_TEST_SOURCE_FILES
        cxtestSonixProbeFileReader.cpp
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtestGrabber ${CX_TEST_SOURCE_FILES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtestGrabber)
    target_include_directories(cxtestGrabber
        PUBLIC
        .
        ..
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtestGrabber PRIVATE cxGrabber cxCatch cxtestUtilities)
    cx_add_tests_to_catch(cxtestGrabber)

endif(BUILD_TESTING)
