
if(BUILD_TESTING)

    set(RESOURCE_OPENIGTLINKUTILITIES_TEST_CATCH_SOURCE_FILES
        cxtestCatchIGTLinkConversion.cpp
        cxtestIGTLinkConversionFixture.h
        cxtestIGTLinkConversionFixture.cpp
    )

    #qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtestOpenIGTLinkUtilities ${RESOURCE_OPENIGTLINKUTILITIES_TEST_CATCH_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtestOpenIGTLinkUtilities)
    target_include_directories(cxtestOpenIGTLinkUtilities
        PUBLIC
        .
        ..
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtestOpenIGTLinkUtilities PRIVATE cxOpenIGTLinkUtilities cxCatch)
    cx_add_tests_to_catch(cxtestOpenIGTLinkUtilities)

endif(BUILD_TESTING)
