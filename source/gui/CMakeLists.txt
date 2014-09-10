###########################################################
##               Gui library
###########################################################


cx_add_class_qt_moc(cxGui_SRCS
	#----------------------
	dataAdapters/cxDataInterface
	dataAdapters/cxShadingParamsInterfaces
	dataAdapters/cxToolDataAdapters
	dataAdapters/cxDoubleDataAdapterTemporalCalibration
	dataAdapters/cxSelectDataStringDataAdapter
	dataAdapters/cxOptionsWidget
    #----------------------
	dataWidgets/cxVolumePropertiesWidget
	dataWidgets/cxVolumeInfoWidget
	dataWidgets/cxShadingWidget
	dataWidgets/cxCroppingWidget
	dataWidgets/cxClippingWidget
	dataWidgets/cxSlicePropertiesWidget
	dataWidgets/cxMeshInfoWidget
	dataWidgets/cxImportDataDialog
	dataWidgets/cxExportDataDialog
	dataWidgets/cxDataViewSelectionWidget
	dataWidgets/cxTransferFunctionWidget
	dataWidgets/cxTransferFunctionAlphaWidget
	dataWidgets/cxTransferFunctionColorWidget
	dataWidgets/cxEraserWidget
	dataWidgets/cxDataSelectWidget
	dataWidgets/cxInfoWidget
	dataWidgets/cxTransferFunctionPresetWidget
	dataWidgets/cxOverlayWidget
	dataWidgets/cxColorWidget
	dataWidgets/cxTransferFunction2DColorWidget
	dataWidgets/cxTransferFunction2DOpacityWidget
	dataWidgets/cxImagePropertiesWidget
    #----------------------
	infoWidgets/cxStatusBar
	infoWidgets/cxPointSamplingWidget
	infoWidgets/cxMetricWidget
	infoWidgets/cxFrameTreeWidget
	infoWidgets/cxDataMetricWrappers
	infoWidgets/cxSamplerWidget
	infoWidgets/cxFrameMetricWrapper
    infoWidgets/cxToolMetricWrapper
    infoWidgets/cxRegistrationHistoryWidget
	infoWidgets/cxMetricManager

    #----------------------
	navigationWidgets/cxNavigationWidget
	navigationWidgets/cxTrackPadWidget
	navigationWidgets/cxPlaybackWidget
    #----------------------
	preferencesWidgets/cxPreferencesDialog
	preferencesWidgets/cxToolFilterWidget
	preferencesWidgets/cxToolListWidget
	preferencesWidgets/cxToolConfigureWidget
	preferencesWidgets/cxLayoutEditorWidget
	preferencesWidgets/cxGeneralTab
	preferencesWidgets/cxPreferenceTab
    #----------------------
        streamingWidgets/cxVideoConnectionWidget
    #----------------------
	toolWidgets/cxToolPropertiesWidget
	toolWidgets/cxActiveToolWidget
	toolWidgets/cxToolManagerWidget
	toolWidgets/cxProbeConfigWidget
    #----------------------
	utilities/cxAudioImpl
	utilities/cxToolImagePreviewWidget
    #----------------------    
    pluginFrameworkWidgets/cxPluginBrowser
    pluginFrameworkWidgets/cxPluginTableModel
    pluginFrameworkWidgets/cxPluginFrameworkWidget
    #----------------------    
	cxMainWindow
	cxSecondaryMainWindow
	cxSecondaryViewLayoutWindow
	cxLayoutInteractor
    #----------------------
)


# generate rules for building source files from the resources
QT5_ADD_RESOURCES(cxGui_SRCS cxResources.qrc)

cx_add_class(cxGui_SRCS
	${cxGui_SRCS}
	${cxGui_RCC_SRCS}
	dataAdapters/cxDataAdapterHelper
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
    pluginFrameworkWidgets
    )

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${SUB_FOLDERS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)
include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ${CustusX_SOURCE_DIR}/source/resource/videoServer
    ${CustusX_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
    ${CustusX_SOURCE_DIR}/source/service/patient     
    ${CustusX_SOURCE_DIR}/source/service/tracking     
    ${CustusX_SOURCE_DIR}/source/service/visualization
    ${CustusX_SOURCE_DIR}/source/service/video
    ${CustusX_SOURCE_DIR}/source/service/state
    
    ${CustusX_SOURCE_DIR}/source/logic
    ${CustusX_SOURCE_DIR}/source/modules/GUIExtender
    ${CustusX_SOURCE_DIR}/source/resource/pluginUtilities
    )
    
set(CX_GUI_LIBRARIES cxLogicManager cxStateService cxResourceWidgets)
    
add_library( cxGui ${cxGui_SRCS} )
target_link_libraries( cxGui ${CX_GUI_LIBRARIES} )

add_subdirectory(testing)

