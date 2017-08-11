project(org_custusx_dicom)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

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

# needed on windows to where <windows.h> is included
# http://qt-project.org/forums/viewthread/22133
if(WIN32)
    add_definitions(-DNOMINMAX)
endif(WIN32)

#find_package(CTKDICOMCore)
#message(STATUS "**** IMPORTED_LINK_INTERFACE_LIBRARIES_${CMAKE_BUILD_TYPE}" )
set(CX_IMPORT_PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_${CMAKE_BUILD_TYPE})
string(TOUPPER ${CX_IMPORT_PROPERTY} CX_IMPORT_PROPERTY)
get_target_property(TEST_VALUE CTKDICOMCore ${CX_IMPORT_PROPERTY})

#message(STATUS "IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE: ${TEST_VALUE}")
#foreach (var ${TEST_VALUE})
#	message(STATUS "    " ${var})
#endforeach()
#IMPORTED_LOCATION_RELEASE

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${CTK_DCMTK_DIR}/lib)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries 
    ${PLUGIN_target_libraries}
    cxGUIExtenderService
    cxResourceWidgets
    cxResource
    CTKDICOMWidgets
    CTKDICOMCore
    cxPluginUtilities
    cxResourceVisualization
    ${CTK_EXTERNAL_LIBRARIES}
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

target_include_directories(org_custusx_dicom
    PUBLIC
    .
    ./core
    ./widgets
    ${CMAKE_CURRENT_BINARY_DIR}
# These two lines are needed with latest CTK. See cha: 50a0e60f708f3bf4b0913d073b25ef3d90c81bf1
    ${CTK_DIR}/DCMTK/dcmdata/include/dcmtk/dcmdata
    ${CTK_DIR}/DCMTK/dcmimgle/include/dcmtk/dcmimgle
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org.custusx.dicom.md")

add_subdirectory(testing)

