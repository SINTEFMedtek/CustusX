
if(BUILD_TESTING)
    set(CXTEST_SOURCES_TO_MOC
        cxElastixSingleThreadedRunner.h
    )
    set(CXTEST_PLUGINREGISTRATION_SOURCES ${CXTEST_PLUGINREGISTRATION_SOURCES}
        cxElastixSingleThreadedRunner.h
        cxElastixSingleThreadedRunner.cpp
        cxtestCatchElastix.cpp
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtest_org_custusx_registration_method_commandline ${CXTEST_PLUGINREGISTRATION_SOURCES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_registration_method_commandline)
    target_include_directories(cxtest_org_custusx_registration_method_commandline
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_registration_method_commandline
      PRIVATE
      org_custusx_registration_method_commandline
      cxtestResource
      cxCatch
      cxLogicManager)
    cx_add_tests_to_catch(cxtest_org_custusx_registration_method_commandline)

endif(BUILD_TESTING)
