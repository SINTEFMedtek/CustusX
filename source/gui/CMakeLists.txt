###########################################################
##               Gui library
###########################################################

cx_add_class_qt_moc(cxGui_SRCS
    #----------------------
    dataAdapters/cxDataInterface
    dataAdapters/cxShadingParamsInterfaces
    dataAdapters/cxToolDataAdapters
    dataAdapters/cxDoubleDataAdapterTemporalCalibration
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
	utilities/cxDockWidgets
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

cx_add_class(cxGui_SRCS
	${cxGui_SRCS}
	dataAdapters/cxDataAdapterHelper
)

cx_create_export_header("cxGui")

# generate rules for building source files from the resources
qt5_add_resources(cxResources cxResources.qrc)
if(CX_WINDOWS)
    set(cxResources
        ${cxResources}
        cxResources.h
    )
endif(CX_WINDOWS)

add_library( cxGui ${cxGui_SRCS} ${cxResources})
target_include_directories(cxGui
    PUBLIC
    .
    ./dataAdapters
    ./dataWidgets
    ./infoWidgets
    ./navigationWidgets
    ./preferencesWidgets
    ./streamingWidgets
    ./toolWidgets
    ./utilities
    ./pluginFrameworkWidgets
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CustusX_SOURCE_DIR}/source/ThirdParty
)
target_link_libraries(cxGui
    PRIVATE
    cxResource
    cxLogicManager
    cxResourceWidgets
    cxResourceVisualization
    cxGUIExtenderService
    cxPluginUtilities
)

add_subdirectory(testing)

