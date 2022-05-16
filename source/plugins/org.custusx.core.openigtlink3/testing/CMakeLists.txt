
if(BUILD_TESTING)
    set(TEST_LIB_NAME
        cxtest_${PROJECT_NAME}
    )
    set(MOC_SOURCE_FILES
        cxtestReceiver.h
        cxtestPlusReceiver.h
        cxtestIOReceiver.h
    )
    set(SOURCE_FILES
        ${MOC_SOURCE_FILES}
        cxtestReceiver.cpp
        cxtestPlusReceiver.cpp
        cxtestIOReceiver.cpp
        cxtestOpenIGTLinkIO.cpp
        cxtestProbeDefinitionFromStringMessages.cpp
        cxtestOpenIGTLinkTrackingSystemService.cpp
    )

    qt5_wrap_cpp(MOC_SOURCE_FILES ${MOC_SOURCE_FILES})
    add_library(${TEST_LIB_NAME} ${SOURCE_FILES} ${MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(${TEST_LIB_NAME})
    target_include_directories(${TEST_LIB_NAME}
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(
      ${TEST_LIB_NAME}
      PRIVATE
      ${PROJECT_NAME}
      cxtestUtilities
      cxtestResourceVisualization
      cxCatch)
    cx_add_tests_to_catch(${TEST_LIB_NAME})

endif(BUILD_TESTING)
