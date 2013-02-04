###########################################################
##               Gui library
###########################################################


# _USE_MATH_DEFINES Must be defined before math.h is included
if (CX_WINDOWS)
    add_definitions(-D_USE_MATH_DEFINES)
endif(CX_WINDOWS)

#if (APPLE)
#    set(QXT_FOLDER "../ThirdParty/qxt_mac_compatible")
#else()
    set(QXT_FOLDER "../ThirdParty/qxt")
#endif()

# Make VERSION_NUMBER available for c++ files in gui folder
ADD_DEFINITIONS(-DVERSION_NUMBER="${VERSION_NUMBER}")
ADD_DEFINITIONS(-DVERSION_NUMBER_VERBOSE="${VERSION_NUMBER_VERBOSE}")

SET(cxGui_MOC_HDRS
    #----------------------
    dataAdapters/cxDataInterface.h
    dataAdapters/cxShadingParamsInterfaces.h
    dataAdapters/cxToolDataAdapters.h
    dataAdapters/cxDoubleDataAdapterTemporalCalibration.h
    dataAdapters/cxVector3DComponentDataAdapter.h
    #----------------------
    dataWidgets/cxVolumePropertiesWidget.h
    dataWidgets/cxShadingWidget.h
    dataWidgets/cxCroppingWidget.h
    dataWidgets/cxClippingWidget.h 
    dataWidgets/cxImagePropertiesWidget.h
    dataWidgets/cxMeshPropertiesWidget.h
    dataWidgets/cxImportDataDialog.h
    dataWidgets/cxExportDataDialog.h
    dataWidgets/cxDataViewSelectionWidget.h
    dataWidgets/cxTransferFunctionWidget.h
    dataWidgets/cxTransferFunctionAlphaWidget.h
    dataWidgets/cxTransferFunctionColorWidget.h
    dataWidgets/sscDICOMWidget.h
    dataWidgets/cxEraserWidget.h
	dataWidgets/cxDataSelectWidget.h
    #----------------------
    infoWidgets/cxStatusBar.h
    infoWidgets/cxBrowserWidget.h
    infoWidgets/cxPointSamplingWidget.h
    infoWidgets/cxMetricWidget.h
    infoWidgets/cxFrameTreeWidget.h
    infoWidgets/cxTreeModelItem.h   
    infoWidgets/cxDataMetricWrappers.h   
	infoWidgets/cxSamplerWidget.h
    #----------------------
    navigationWidgets/cxNavigationWidget.h
    navigationWidgets/cxTrackPadWidget.h
    navigationWidgets/cxPlaybackWidget.h
    #----------------------
    preferencesWidgets/cxPreferencesDialog.h
    preferencesWidgets/cxToolFilterWidget.h
    preferencesWidgets/cxToolListWidget.h
    preferencesWidgets/cxToolConfigureWidget.h
    preferencesWidgets/cxLayoutEditorWidget.h
    preferencesWidgets/cxLayoutEditorTab.h
    #----------------------
    streamingWidgets/cxIGTLinkWidget.h
    #----------------------
    toolWidgets/cxToolPropertiesWidget.h
    toolWidgets/cxActiveToolWidget.h
    toolWidgets/cxToolManagerWidget.h
    toolWidgets/cxProbeConfigWidget.h
    #----------------------
    utilities/cxAudio.h
    utilities/cxDoubleSpanSlider.h
    utilities/cxBaseWidget.h
    utilities/cxTransform3DWidget.h
    utilities/cxSelectionGroupBox.h
    utilities/cxVector3DWidget.h
    utilities/cxBoundingBoxWidget.h
    utilities/cxFileWatcherWidget.h
    utilities/cxFilePreviewWidget.h
    utilities/cxImagePreviewWidget.h
    utilities/cxToolImagePreviewWidget.h
    #----------------------    
    cxMainWindow.h
    cxSecondaryMainWindow.h
    #----------------------
    ${QXT_FOLDER}/qxtspanslider_p.h
    ${QXT_FOLDER}/qxtspanslider.h

)

set(SUB_FOLDERS
    dataAdapters
    dataWidgets
    infoWidgets
    navigationWidgets
    preferencesWidgets
    streamingWidgets
    toolWidgets
    utilities
    ${QXT_FOLDER}
    )

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${SUB_FOLDERS}
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(
    .
    ${SSC_SOURCE_DIR}/DICOMLib/
    ${SSC_BINARY_DIR}/DICOMLib/

    ${CustusX3_SOURCE_DIR}/modules/OpenIGTLinkServer

    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms

    ${CustusX3_SOURCE_DIR}/source/service/patient     
    ${CustusX3_SOURCE_DIR}/source/service/tracking     
    ${CustusX3_SOURCE_DIR}/source/service/visualization
    ${CustusX3_SOURCE_DIR}/source/service/video
    ${CustusX3_SOURCE_DIR}/source/service/state
    
    ${CustusX3_SOURCE_DIR}/source/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    )


# After this call, cxProject_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx ...
IF(CX_WINDOWS)
  QT4_WRAP_CPP(cxGui_MOC_SRCS ${cxGui_MOC_HDRS} OPTIONS "-D_WINDOWS")
ELSE(CX_WINDOWS)
  QT4_WRAP_CPP(cxGui_MOC_SRCS ${cxGui_MOC_HDRS} )
ENDIF(CX_WINDOWS)

SET(cxGui_RCCS cxResources.qrc)

# generate rules for building source files from the resources
QT4_ADD_RESOURCES(cxGui_RCC_SRCS ${cxGui_RCCS})

SET(cxGui_SRCS
    ${cxGui_MOC_HDRS}
    #----------------------
    dataAdapters/cxDataInterface.h
    dataAdapters/cxDataInterface.cpp
    dataAdapters/cxShadingParamsInterfaces.h
    dataAdapters/cxShadingParamsInterfaces.cpp
    dataAdapters/cxToolDataAdapters.h
    dataAdapters/cxToolDataAdapters.cpp
    dataAdapters/cxDoubleDataAdapterTemporalCalibration.h
    dataAdapters/cxDoubleDataAdapterTemporalCalibration.cpp    
    dataAdapters/cxVector3DComponentDataAdapter.h
    dataAdapters/cxVector3DComponentDataAdapter.cpp
    #----------------------
    dataWidgets/cxVolumePropertiesWidget.h
    dataWidgets/cxVolumePropertiesWidget.cpp
    dataWidgets/cxShadingWidget.h
    dataWidgets/cxShadingWidget.cpp
    dataWidgets/cxCroppingWidget.h
    dataWidgets/cxCroppingWidget.cpp
    dataWidgets/cxClippingWidget.h  
    dataWidgets/cxClippingWidget.cpp
    dataWidgets/cxTransferFunctionWidget.h
    dataWidgets/cxTransferFunctionWidget.cpp
    dataWidgets/cxTransferFunctionAlphaWidget.h
    dataWidgets/cxTransferFunctionAlphaWidget.cpp
    dataWidgets/cxTransferFunctionColorWidget.h
    dataWidgets/cxTransferFunctionColorWidget.cpp
    dataWidgets/cxImagePropertiesWidget.h
    dataWidgets/cxImagePropertiesWidget.cpp
    dataWidgets/cxMeshPropertiesWidget.h
    dataWidgets/cxMeshPropertiesWidget.cpp
    dataWidgets/cxImportDataDialog.h
    dataWidgets/cxImportDataDialog.cpp
    dataWidgets/cxExportDataDialog.h
    dataWidgets/cxExportDataDialog.cpp
    dataWidgets/cxDataViewSelectionWidget.h
    dataWidgets/cxDataViewSelectionWidget.cpp
    dataWidgets/sscDICOMWidget.h
    dataWidgets/sscDICOMWidget.cpp
    dataWidgets/cxEraserWidget.h
    dataWidgets/cxEraserWidget.cpp
	dataWidgets/cxDataSelectWidget.h
	dataWidgets/cxDataSelectWidget.cpp
	#----------------------
    infoWidgets/cxBrowserWidget.h
    infoWidgets/cxBrowserWidget.cpp
    infoWidgets/cxFrameTreeWidget.cpp
    infoWidgets/cxFrameTreeWidget.cpp
    infoWidgets/cxPointSamplingWidget.h
    infoWidgets/cxPointSamplingWidget.cpp
    infoWidgets/cxStatusBar.h
    infoWidgets/cxStatusBar.cpp    
    infoWidgets/cxTreeModelItem.h
    infoWidgets/cxTreeModelItem.cpp
    infoWidgets/cxMetricWidget.h
    infoWidgets/cxMetricWidget.cpp
    infoWidgets/cxDataMetricWrappers.h   
    infoWidgets/cxDataMetricWrappers.cpp   
	infoWidgets/cxSamplerWidget.h
	infoWidgets/cxSamplerWidget.cpp
    #----------------------
    navigationWidgets/cxNavigationWidget.h
    navigationWidgets/cxNavigationWidget.cpp
    navigationWidgets/cxTrackPadWidget.cpp
    navigationWidgets/cxTrackPadWidget.h  
    navigationWidgets/cxPlaybackWidget.h
    navigationWidgets/cxPlaybackWidget.cpp
    #----------------------
    preferencesWidgets/cxPreferencesDialog.h
    preferencesWidgets/cxPreferencesDialog.cpp
    preferencesWidgets/cxToolFilterWidget.h
    preferencesWidgets/cxToolFilterWidget.cpp
    preferencesWidgets/cxToolListWidget.h
    preferencesWidgets/cxToolListWidget.cpp
    preferencesWidgets/cxToolConfigureWidget.h
    preferencesWidgets/cxToolConfigureWidget.cpp
    preferencesWidgets/cxLayoutEditorWidget.h
    preferencesWidgets/cxLayoutEditorWidget.cpp
    preferencesWidgets/cxLayoutEditorTab.h
    preferencesWidgets/cxLayoutEditorTab.cpp
    #----------------------    
    streamingWidgets/cxIGTLinkWidget.h
    streamingWidgets/cxIGTLinkWidget.cpp
    #----------------------    
    toolWidgets/cxToolPropertiesWidget.h
    toolWidgets/cxToolPropertiesWidget.cpp
    toolWidgets/cxActiveToolWidget.h
    toolWidgets/cxActiveToolWidget.cpp
    toolWidgets/cxToolManagerWidget.h
    toolWidgets/cxToolManagerWidget.cpp
    toolWidgets/cxProbeConfigWidget.h
    toolWidgets/cxProbeConfigWidget.cpp
    #----------------------    
    utilities/cxDoubleSpanSlider.h
    utilities/cxDoubleSpanSlider.cpp
    utilities/cxTabbedWidget.h
    utilities/cxTabbedWidget.cpp
    utilities/cxBaseWidget.h
    utilities/cxBaseWidget.cpp
    utilities/cxAudio.h
    utilities/cxAudio.cpp
    utilities/cxTransform3DWidget.h
    utilities/cxTransform3DWidget.cpp
    utilities/snwSyntaxHighlighter.cpp
    utilities/cxSelectionGroupBox.h
    utilities/cxSelectionGroupBox.cpp
    utilities/cxVector3DWidget.h
    utilities/cxVector3DWidget.cpp    
    utilities/cxBoundingBoxWidget.h
    utilities/cxBoundingBoxWidget.cpp
    utilities/cxFileWatcherWidget.h
    utilities/cxFileWatcherWidget.cpp
    utilities/cxFilePreviewWidget.h
    utilities/cxFilePreviewWidget.cpp
    utilities/cxImagePreviewWidget.h
    utilities/cxImagePreviewWidget.h
    utilities/cxImagePreviewWidget.cpp
    utilities/cxToolImagePreviewWidget.h
    utilities/cxToolImagePreviewWidget.cpp
    #----------------------    
    cxMainWindow.h
    cxMainWindow.cpp 
    cxSecondaryMainWindow.h
    cxSecondaryMainWindow.cpp
    #----------------------    
    ${QXT_FOLDER}/qxtspanslider_p.h
    ${QXT_FOLDER}/qxtspanslider.h
    ${QXT_FOLDER}/qxtspanslider.cpp    
    #----------------------
    ${cxGui_RCC_SRCS} )
    
set(CX_GUI_LIBRARIES cxLogicManager cxStateService)
if (SSC_USE_DCMTK)
    set(CX_GUI_LIBRARIES ${CX_GUI_LIBRARIES} DICOMLib)
endif (SSC_USE_DCMTK)
    
add_library( cxGui ${cxGui_SRCS} ${cxGui_MOC_SRCS} )
target_link_libraries( cxGui ${CX_GUI_LIBRARIES} )

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxGui)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxGui_unity 
        		${CX_GUI_LIBRARIES}
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)

