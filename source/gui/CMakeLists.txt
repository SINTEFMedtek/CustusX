SET(LIBNAME gui)

# _USE_MATH_DEFINES Must be defined before math.h is included
if (WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif(WIN32)

# Make VERSION_NUMBER available for c++ files in gui folder
ADD_DEFINITIONS(-DVERSION_NUMBER="${VERSION_NUMBER}")
ADD_DEFINITIONS(-DVERSION_NUMBER_VERBOSE="${VERSION_NUMBER_VERBOSE}")

SET(cxProject_MOC_HDRS
    #----------------------
    algorithmWidgets/cxBinaryThresholdImageFilterWidget.h
    algorithmWidgets/cxToolTipCalibrationWidget.h
    algorithmWidgets/cxConnectedThresholdImageFilterWidget.h
    algorithmWidgets/cxSurfaceWidget.h
    algorithmWidgets/cxCenterlineWidget.h
    algorithmWidgets/cxResampleWidget.h
    algorithmWidgets/cxToolTipSampleWidget.h
    algorithmWidgets/cxToolManualCalibrationWidget.h
    algorithmWidgets/cxTemporalCalibrationWidget.h
#    algorithmWidgets/cxCalibrationMethodsWidget.h
#    algorithmWidgets/cxVisualizationMethodsWidget.h
#    algorithmWidgets/cxSegmentationMethodsWidget.h
    #----------------------
    dataAdapters/cxDataInterface.h
    dataAdapters/cxShadingParamsInterfaces.h
    dataAdapters/cxToolDataAdapters.h
    dataAdapters/cxDoubleDataAdapterTemporalCalibration.h
    #----------------------
    dataWidgets/cxVolumePropertiesWidget.h
    dataWidgets/cxShadingWidget.h
    dataWidgets/cxCroppingWidget.h
    dataWidgets/cxClippingWidget.h 
    dataWidgets/cxImagePropertiesWidget.h
    dataWidgets/cxMeshPropertiesWidget.h
    dataWidgets/cxImportDataDialog.h
    dataWidgets/cxDataViewSelectionWidget.h
    dataWidgets/cxTransferFunctionWidget.h
    dataWidgets/cxTransferFunctionAlphaWidget.h
    dataWidgets/cxTransferFunctionColorWidget.h
    #----------------------
    infoWidgets/cxStatusBar.h
    infoWidgets/cxBrowserWidget.h
    infoWidgets/cxPointSamplingWidget.h
    infoWidgets/cxFrameTreeWidget.h
    infoWidgets/cxTreeModelItem.h   
    #----------------------
    navigationWidgets/cxNavigationWidget.h
    navigationWidgets/cxTrackPadWidget.h
    #----------------------
    preferencesWidgets/cxPreferencesDialog.h
    preferencesWidgets/cxToolFilterWidget.h
    preferencesWidgets/cxToolListWidget.h
    preferencesWidgets/cxToolConfigureWidget.h
    preferencesWidgets/cxLayoutEditorWidget.h
    preferencesWidgets/cxLayoutEditorTab.h
    #----------------------
    recordingWidgets/cxRecordSessionWidget.h
    recordingWidgets/cxRecordBaseWidget.h
    recordingWidgets/cxTrackedCenterlineWidget.h
    recordingWidgets/cxUSAcqusitionWidget.h
    recordingWidgets/cxSoundSpeedConversionWidget.h
    #----------------------
    registrationWidgets/cxImageRegistrationWidget.h
    registrationWidgets/cxRegistrationWidget.h
    registrationWidgets/cxPatientRegistrationWidget.h
    registrationWidgets/cxRegistrationHistoryWidget.h
    registrationWidgets/cxManualRegistrationOffsetWidget.h
    registrationWidgets/cxFastOrientationRegistrationWidget.h
    registrationWidgets/cxImageSegmentationAndCenterlineWidget.h
    registrationWidgets/cxPlateRegistrationWidget.h
    registrationWidgets/cxSeansVesselRegistrationWidget.h
    registrationWidgets/cxRegisterI2IWidget.h
    #registrationWidgets/cxRegistrationMethodsWidget.h
    #----------------------
    streamingWidgets/cxIGTLinkWidget.h
    #----------------------
    toolWidgets/cxToolPropertiesWidget.h
    toolWidgets/cxActiveToolWidget.h
    toolWidgets/cxToolManagerWidget.h
    #----------------------
    utilities/cxDoubleSpanSlider.h
    utilities/cxBaseWidget.h
    utilities/cxTransform3DWidget.h
    utilities/cxFilePreviewWidget.h
    utilities/cxSelectionGroupBox.h
    utilities/cxColorSelectButton.h
    #----------------------    
    cxMainWindow.h
    cxSecondaryMainWindow.h
    #----------------------
    ../ThirdParty/qxt/qxtspanslider_p.h
    ../ThirdParty/qxt/qxtspanslider.h

)

# After this call, cxProject_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx ...
IF(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${cxProject_MOC_HDRS} OPTIONS "-D_WINDOWS")
ELSE(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${cxProject_MOC_HDRS} )
ENDIF(WIN32)

SET(cxProject_RCCS cxResources.qrc)

# generate rules for building source files from the resources
QT4_ADD_RESOURCES(cxProject_RCC_SRCS ${cxProject_RCCS})

SET(${LIBNAME}_SRCS
    ${cxProject_MOC_HDRS}
    #----------------------    
    algorithmWidgets/cxBinaryThresholdImageFilterWidget.h
    algorithmWidgets/cxBinaryThresholdImageFilterWidget.cpp
    algorithmWidgets/cxToolTipCalibrationWidget.h
    algorithmWidgets/cxToolTipCalibrationWidget.cpp
    algorithmWidgets/cxConnectedThresholdImageFilterWidget.h
    algorithmWidgets/cxConnectedThresholdImageFilterWidget.cpp
    algorithmWidgets/cxSurfaceWidget.cpp
    algorithmWidgets/cxSurfaceWidget.h
    algorithmWidgets/cxCenterlineWidget.cpp
    algorithmWidgets/cxCenterlineWidget.h
    algorithmWidgets/cxResampleWidget.cpp
    algorithmWidgets/cxResampleWidget.h
    algorithmWidgets/cxToolTipSampleWidget.h
    algorithmWidgets/cxToolTipSampleWidget.cpp
    algorithmWidgets/cxToolManualCalibrationWidget.h
    algorithmWidgets/cxToolManualCalibrationWidget.cpp
    algorithmWidgets/cxTemporalCalibrationWidget.h
    algorithmWidgets/cxTemporalCalibrationWidget.cpp
    algorithmWidgets/cxCalibrationMethodsWidget.h
    algorithmWidgets/cxCalibrationMethodsWidget.cpp
    algorithmWidgets/cxVisualizationMethodsWidget.h
    algorithmWidgets/cxVisualizationMethodsWidget.cpp
    algorithmWidgets/cxSegmentationMethodsWidget.h
    algorithmWidgets/cxSegmentationMethodsWidget.cpp
    #----------------------
    dataAdapters/cxDataInterface.h
    dataAdapters/cxDataInterface.cpp
    dataAdapters/cxShadingParamsInterfaces.h
    dataAdapters/cxShadingParamsInterfaces.cpp
    dataAdapters/cxToolDataAdapters.h
    dataAdapters/cxToolDataAdapters.cpp
    dataAdapters/cxDoubleDataAdapterTemporalCalibration.h
    dataAdapters/cxDoubleDataAdapterTemporalCalibration.cpp    
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
    dataWidgets/cxDataViewSelectionWidget.h
    dataWidgets/cxDataViewSelectionWidget.cpp
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
    #----------------------
    navigationWidgets/cxNavigationWidget.h
    navigationWidgets/cxNavigationWidget.cpp
    navigationWidgets/cxTrackPadWidget.cpp
    navigationWidgets/cxTrackPadWidget.h  
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
    recordingWidgets/cxRecordSessionWidget.h
    recordingWidgets/cxRecordSessionWidget.cpp
    recordingWidgets/cxRecordBaseWidget.h
    recordingWidgets/cxRecordBaseWidget.cpp
    recordingWidgets/cxTrackedCenterlineWidget.h
    recordingWidgets/cxTrackedCenterlineWidget.cpp
    recordingWidgets/cxUSAcqusitionWidget.h
    recordingWidgets/cxUSAcqusitionWidget.cpp
    recordingWidgets/cxSoundSpeedConversionWidget.h
    recordingWidgets/cxSoundSpeedConversionWidget.cpp
    #----------------------    
    registrationWidgets/cxImageRegistrationWidget.h
    registrationWidgets/cxImageRegistrationWidget.cpp
    registrationWidgets/cxRegistrationWidget.h
    registrationWidgets/cxRegistrationWidget.cpp
    registrationWidgets/cxPatientRegistrationWidget.h
    registrationWidgets/cxPatientRegistrationWidget.cpp
    registrationWidgets/cxRegistrationHistoryWidget.h
    registrationWidgets/cxRegistrationHistoryWidget.cpp
    registrationWidgets/cxManualRegistrationOffsetWidget.h
    registrationWidgets/cxManualRegistrationOffsetWidget.cpp
    registrationWidgets/cxFastImageRegistrationWidget.h
    registrationWidgets/cxFastImageRegistrationWidget.cpp
    registrationWidgets/cxFastPatientRegistrationWidget.h
    registrationWidgets/cxFastPatientRegistrationWidget.cpp
    registrationWidgets/cxFastOrientationRegistrationWidget.h
    registrationWidgets/cxFastOrientationRegistrationWidget.cpp
    registrationWidgets/cxImageSegmentationAndCenterlineWidget.h
    registrationWidgets/cxImageSegmentationAndCenterlineWidget.cpp
    registrationWidgets/cxPlateRegistrationWidget.h
    registrationWidgets/cxPlateRegistrationWidget.cpp
    registrationWidgets/cxSeansVesselRegistrationWidget.h
    registrationWidgets/cxSeansVesselRegistrationWidget.cpp
    registrationWidgets/cxRegisterI2IWidget.h
    registrationWidgets/cxRegisterI2IWidget.cpp    
    registrationWidgets/cxRegistrationMethodsWidget.h
    registrationWidgets/cxRegistrationMethodsWidget.cpp
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
    #----------------------    
    utilities/cxDoubleSpanSlider.h
    utilities/cxDoubleSpanSlider.cpp
    utilities/cxTabbedWidget.h
    utilities/cxTabbedWidget.cpp
    utilities/cxBaseWidget.h
    utilities/cxAudio.h
    utilities/cxAudio.cpp
    utilities/cxTransform3DWidget.h
    utilities/cxTransform3DWidget.cpp
    utilities/cxFilePreviewWidget.h
    utilities/cxFilePreviewWidget.cpp
    utilities/snwSyntaxHighlighter.cpp
    utilities/cxSelectionGroupBox.h
    utilities/cxSelectionGroupBox.cpp
    utilities/cxColorSelectButton.h
    utilities/cxColorSelectButton.cpp
    
    #----------------------    
    cxMainWindow.h
    cxMainWindow.cpp 
    cxSecondaryMainWindow.h
    cxSecondaryMainWindow.cpp
    #----------------------    
    ../ThirdParty/qxt/qxtspanslider_p.h
    ../ThirdParty/qxt/qxtspanslider.h
    ../ThirdParty/qxt/qxtspanslider.cpp    
    #----------------------
    ${cxProject_RCC_SRCS} )


set(SUB_FOLDERS
    algorithmWidgets
    dataAdapters
    dataWidgets
    infoWidgets
    navigationWidgets
    preferencesWidgets
    recordingWidgets
    registrationWidgets
    streamingWidgets
    toolWidgets
    utilities
    ../ThirdParty/qxt
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
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/logic/stateMachineManager    
    ${CustusX3_SOURCE_DIR}/source/registrationManager    
#    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters     
    ${CustusX3_SOURCE_DIR}/source/service/patient     
    ${CustusX3_SOURCE_DIR}/source/service/tracking     
    ${CustusX3_SOURCE_DIR}/source/service/visualization
    ${CustusX3_SOURCE_DIR}/source/service/video
    ${CustusX3_SOURCE_DIR}/source/logic
    ${CustusX3_SOURCE_DIR}/source/algorithms
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms
    ${CustusX3_SOURCE_DIR}/source/UsReconstruction
    
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/gui
    )


ADD_LIBRARY ( ${LIBNAME} ${${LIBNAME}_SRCS}  ${${LIBNAME}_MOC_SRCS} )
target_link_libraries(${LIBNAME} registrationManager cxPluginUSReconstructionGUI statemachineManager cxLogicManager)

