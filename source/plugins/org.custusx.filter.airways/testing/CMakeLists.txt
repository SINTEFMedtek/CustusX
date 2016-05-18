
if(BUILD_TESTING)
    cx_add_class(CXTEST_SOURCES ${CXTEST_SOURCES}
		cxtestAirwaysFixture
		cxtestAirways.cpp
    )
    set(CXTEST_SOURCES_TO_MOC
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtest_org_custusx_filter_airways ${CXTEST_SOURCES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_filter_airways)
    target_include_directories(cxtest_org_custusx_filter_airways
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_filter_airways
        PRIVATE
		cxtestResource
        cxResource
        cxResourceFilter
        cxLogicManager
		OpenCLUtilityLibrary
		FAST
        org_custusx_filter_airways
        cxCatch
    )
    cx_add_tests_to_catch(cxtest_org_custusx_filter_airways)

endif(BUILD_TESTING)
