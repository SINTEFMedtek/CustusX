
if(BUILD_TESTING)
  project(cxtest_org_custusx_core_view)

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
  add_library(${PROJECT_NAME} ${CX_TEST_CATCH_org_custusx_core_view_SOURCE_FILES} ${CX_TEST_CATCH_org_custusx_core_view_MOC_SOURCE_FILES})
  include(GenerateExportHeader)
  generate_export_header(${PROJECT_NAME})
  target_include_directories(${PROJECT_NAME}
    PUBLIC
    .
    ..
    ${CMAKE_CURRENT_BINARY_DIR}
    )
  target_link_libraries(${PROJECT_NAME}
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
  cx_add_tests_to_catch(${PROJECT_NAME})

# This fixes crash when trying to use vtkSmartVolumeMapper
#  if (NOT VTK_VERSION VERSION_LESS "8.90.0")
  vtk_module_autoinit(
    TARGETS ${PROJECT_NAME}
    MODULES ${VTK_LIBRARIES}
#    MODULES VTK::RenderingVolumeOpenGL2
    )
#  endif()

endif(BUILD_TESTING)
