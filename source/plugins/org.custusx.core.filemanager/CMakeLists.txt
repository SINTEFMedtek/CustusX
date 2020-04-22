###########################################################
##               FileManager Plugin
##
##               Plugin
##
##  Description: Managing reading and writing of files in CustusX
##
##
###########################################################
project(org_custusx_core_filemanager)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
    cxFileManagerServicePluginActivator.cpp
    cxFileManagerImplService.h
    cxFileManagerImplService.cpp

    cxImportExportGuiExtenderService.h
    cxImportExportGuiExtenderService.cpp

    gui/cxExportWidget.cpp
    gui/cxImportWidget.h
    gui/cxImportWidget.cpp
    gui/cxExportDataTypeWidget.h
    gui/cxExportDataTypeWidget.cpp
    gui/cxImportDataTypeWidget.h
    gui/cxImportDataTypeWidget.cpp

    dicom/cxDicomConverter.h
    dicom/cxDicomConverter.cpp
    dicom/cxDicomImageReader.h
    dicom/cxDicomImageReader.cpp

    filereaderwriters/cxMetaImageReader.h
    filereaderwriters/cxMetaImageReader.cpp
    filereaderwriters/cxPNGImageReader.h
    filereaderwriters/cxPNGImageReader.cpp
    filereaderwriters/cxPolyDataMeshReader.h
    filereaderwriters/cxPolyDataMeshReader.cpp
    filereaderwriters/cxXMLPolyDataMeshReader.h
    filereaderwriters/cxXMLPolyDataMeshReader.cpp
    filereaderwriters/cxStlMeshReader.h
    filereaderwriters/cxStlMeshReader.cpp
    filereaderwriters/cxNIfTIReader.h
    filereaderwriters/cxNIfTIReader.cpp
    filereaderwriters/cxMNIReaderWriter.h
    filereaderwriters/cxMNIReaderWriter.cpp
    filereaderwriters/cxDICOMReader.h
    filereaderwriters/cxDICOMReader.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxFileManagerServicePluginActivator.h

    gui/cxExportWidget.h
    gui/cxExportDataTypeWidget.h
    gui/cxImportWidget.h
    gui/cxImportDataTypeWidget.h
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
    cxResource
    cxResourceWidgets
    cxPluginUtilities
    cxGUIExtenderService
    cxGui
    cxResourceVisualization
    CTKDICOMCore
)

set(PLUGIN_OUTPUT_DIR "")
if(CX_WINDOWS)
    #on windows we want dlls to be placed with the executables
    set(PLUGIN_OUTPUT_DIR "../")
endif(CX_WINDOWS)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  OUTPUT_DIR ${PLUGIN_OUTPUT_DIR}
  ${CX_CTK_PLUGIN_NO_INSTALL}
)

target_include_directories( org_custusx_core_filemanager
    PUBLIC
    .
    ./filereaderwriters
    ./gui
    ./dicom
    ${CMAKE_CURRENT_BINARY_DIR}
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org.custusx.core.filemanager.md")
cx_add_non_source_file("doc/org.custusx.core.filemanager.import.md")
cx_add_non_source_file("doc/org.custusx.core.filemanager.export.md")

add_subdirectory(testing)
