
if(BUILD_TESTING)

  set(CX_TEST_CATCH_org_custusx_core_view_MOC_SOURCE_FILES
    cxtestViewCollectionWidgetMixedMock.h
    cxtestLayoutWidgetUsingViewWidgetsMock.h
    cxViewsFixture.h
    )
  set(CX_TEST_CATCH_org_custusx_core_view_SOURCE_FILES
    cxtestViewServiceMock.h
    cxtestViewPlugin.cpp
    cxtestCatchMultiVolume3DRepProducer.cpp
    cxtestMultiVolume3DRepProducerFixture.h
    cxtestMultiVolume3DRepProducerFixture.cpp
    cxtestCatchViewRenderSpeed.cpp
    cxtestCatchVolumeReps.cpp
    cxtestCatchVtkOpenGLGPUMultiVolumeRayCastMapper.cpp
    cxtestDataTypeSort.cpp
    cxtestRendering.cpp
    cxtestVisualizationHelper.h
    cxtestVisualizationHelper.cpp
    cxtestViewGroup.cpp
    cxtestNavigationAlgorithms.cpp
    cxtestViewService.cpp
    cxtestViewServiceMock.cpp
    cxtestViewCollectionWidgetMixedMock.cpp
    cxtestLayoutWidgetUsingViewWidgetsMock.cpp
    cxtestVisualRendering.cpp
    cxtestGeometricRep.cpp
    cxViewsFixture.cpp
	)
  qt5_wrap_cpp(CX_TEST_CATCH_org_custusx_core_view_MOC_SOURCE_FILES ${CX_TEST_CATCH_org_custusx_core_view_MOC_SOURCE_FILES})
  add_library(cxtest_org_custusx_core_view ${CX_TEST_CATCH_org_custusx_core_view_SOURCE_FILES} ${CX_TEST_CATCH_org_custusx_core_view_MOC_SOURCE_FILES})
  include(GenerateExportHeader)
  generate_export_header(cxtest_org_custusx_core_view)
  target_include_directories(cxtest_org_custusx_core_view
    PUBLIC
    .
    ..
    ${CMAKE_CURRENT_BINARY_DIR}
    )
  target_link_libraries(cxtest_org_custusx_core_view
    PRIVATE
    org_custusx_core_view
    cxtestResourceVisualization
    cxtestUtilities
    cxtestResource
    cxCatch
    cxLogicManager
    cxGUIExtenderService
    org_custusx_core_filemanager
    )
  cx_add_tests_to_catch(cxtest_org_custusx_core_view)

endif(BUILD_TESTING)
