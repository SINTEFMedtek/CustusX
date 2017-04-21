
if(BUILD_TESTING)

    set(CXTEST_SOURCES_TO_MOC
    )

    set(CXTEST_SOURCES
        cxtestOptionsWidget.cpp
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtestResourceWidgets ${CXTEST_SOURCES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtestResourceWidgets)
    target_include_directories(cxtestResourceWidgets
        PUBLIC
        .
        ..
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtestResourceWidgets
        PRIVATE
        cxtestUtilities
        cxtestResource
        cxResource
        cxResourceWidgets
        cxResourceVisualization
        cxCatch
    )
    cx_add_tests_to_catch(cxtestResourceWidgets)
    
endif(BUILD_TESTING)
