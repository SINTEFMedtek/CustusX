
if(BUILD_TESTING)

    set(CXTEST_SOURCES_TO_MOC
    )

    set(RESOURCE_TEST_CATCH_SOURCE_FILES
        cxtestXmlOptionFile.cpp
        cxtestCatchStringHelpers.cpp
        cxtestCatchSliceComputer.cpp
        cxtestCatchBoundingBox3D.cpp
        cxtestCatchFrame.cpp
        cxtestCatchSharedMemory.cpp
        cxtestCatchTransform3D.cpp
        cxtestCatchVector3D.cpp
        cxtestImageParameters.cpp
        cxtestCatchImageAlgorithms.cpp
        cxtestCatchProcessWrapper.cpp
        cxtestProcessWrapperFixture.h
        cxtestProcessWrapperFixture.cpp
        cxtestCatchVLCRecorder.cpp
        cxtestVLCRecorderFixture.h
        cxtestVLCRecorderFixture.cpp
        cxtestProbeDefinition.cpp
        cxtestSpaceProviderMock.h
        cxtestSpaceProviderMock.cpp
        cxtestSpaceListenerMock.h
        cxtestSpaceListenerMock.cpp
        cxtestTrackingPositionFilter.cpp
        cxtestCoreServices.cpp
        cxtestReporter.cpp
        cxtestImage.cpp
        cxtestPatientModelServiceMock.cpp
        cxtestPatientModelServiceMock.h
        cxtestVisServices.h
        cxtestVisServices.cpp
        cxtestActiveData.cpp
        cxtestStreamedTimestampSynchronizer.cpp
        cxtestTestDataStructures.h
        cxtestTestDataStructures.cpp
        cxtestDataLocations.cpp
        cxtestDataReaderWriter.cpp
        cxtestTrackingSystemService.cpp
    )

    set(CX_TESTRESOURCE_LINK_LIBS
        cxResource
        cxResourceVisualization
        cxtestUtilities
    )

    if(CX_USE_OPENCL_UTILITY)
        set(RESOURCE_TEST_CATCH_SOURCE_FILES
            ${RESOURCE_TEST_CATCH_SOURCE_FILES}
            cxtestOpenCLPrinter.cpp
        )

        set(OUL_RESOURCE_TEST_CATCH_SOURCE_FILES
            ${OpenCLUtilityLibrary_SOURCE_DIR}/testing/tests.cpp
            ${OpenCLUtilityLibrary_SOURCE_DIR}/testing/histogramPyramidTests.cpp
            )
        set(CX_TESTRESOURCE_LINK_LIBS
            ${CX_TESTRESOURCE_LINK_LIBS}
            TestOpenCLUtilityLibrary
            OpenCLUtilityLibrary
            ${OPENCL_LIBRARIES}
        )
    endif(CX_USE_OPENCL_UTILITY)

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtestResource ${RESOURCE_TEST_CATCH_SOURCE_FILES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtestResource)
    target_include_directories(cxtestResource
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtestResource
        PRIVATE
        ${CX_TESTRESOURCE_LINK_LIBS}
        cxCatch
    )
    cx_add_tests_to_catch(cxtestResource)
    
endif(BUILD_TESTING)
