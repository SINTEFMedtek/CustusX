project(org_custusx_dicom)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/modules/GUIExtender
    ./core
    ./widgets
    )

set(PLUGIN_SRCS
  cxDicomPluginActivator.cpp
  cxDicomGUIExtenderService.cpp
  cxDicomGUIExtenderService.h

  core/cxDicomConverter.h
  core/cxDicomConverter.cpp
  core/cxDicomImageReader.h
  core/cxDicomImageReader.cpp
  
  widgets/cxDicomImporter.cpp
  widgets/cxDicomWidget.cpp
  widgets/cxDICOMAppWidget.cpp
  widgets/cxDICOMModel.cpp
  widgets/cxDicomModelNode.cpp
  widgets/cxDicomModelNode.h
  widgets/cxDICOMThumbnailListWidget.cpp
  widgets/cxDICOMThumbnailGenerator.cpp
  )

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxDicomPluginActivator.h

  widgets/cxDicomImporter.h
  widgets/cxDICOMAppWidget.h
  widgets/cxDICOMModel.h
  widgets/cxDicomWidget.h
  widgets/cxDICOMThumbnailListWidget.h
  widgets/cxDICOMThumbnailGenerator.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)


#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries 
    ${PLUGIN_target_libraries}   
    cxGUIExtenderService
    cxResourceWidgets
    CTKDICOMWidgets
    )

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)

add_subdirectory(testing)

