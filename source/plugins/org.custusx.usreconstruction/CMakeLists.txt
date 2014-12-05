project(org_custusx_usreconstruction)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/modules/GUIExtender
    ${CustusX_SOURCE_DIR}/source/logic
    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/gui
    ${CustusX_BINARY_DIR}/source/modules/Algorithm
    ${CustusX_BINARY_DIR}/source/modules/GUIExtender
    ${CustusX_BINARY_DIR}/source/logic
)

set(PLUGIN_SRCS
    cxUsReconstructionPluginActivator.cpp
	cxUsReconstructionGUIExtenderService.h
	cxUsReconstructionGUIExtenderService.cpp
    cxUsReconstructionImplService.cpp
    cxUsReconstructionService.cpp
    cxUsReconstructionServiceNull.cpp
    cxUsReconstructionServiceProxy.cpp
    cxReconstructThreads.h
    cxReconstructThreads.cpp
    cxReconstructParams.h
    cxReconstructParams.cpp
    cxReconstructionExecuter.cpp
    cxReconstructedOutputVolumeParams.h
    cxReconstructedOutputVolumeParams.cpp
    cxReconstructCore.cpp
    cxReconstructCore.h
    cxReconstructPreprocessor.cpp
    cxReconstructPreprocessor.h
    cxReconstructionWidget.cpp
    cxReconstructOutputValueParamsInterfaces.cpp
    cxReconstructOutputValueParamsInterfaces.h
    cxReconstructionMethodService.h
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxUsReconstructionPluginActivator.h
    cxUsReconstructionImplService.h
    cxUsReconstructionService.h
    cxUsReconstructionServiceNull.h
    cxUsReconstructionServiceProxy.h

#   cxReconstructionManager.h
   cxReconstructThreads.h
   cxReconstructParams.h
   cxReconstructionExecuter.h
   cxReconstructionMethodService.h
   cxReconstructionWidget.h
   cxReconstructOutputValueParamsInterfaces.h
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
    cxPluginUtilities
    cxPluginAlgorithm
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
)

add_subdirectory(testing)

