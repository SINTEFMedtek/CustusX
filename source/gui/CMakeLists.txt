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
	utilities/cxDockWidgets
	)

set(SUB_FOLDERS
    ./dataAdapters
    ./dataWidgets
    ./infoWidgets
    ./navigationWidgets
    ./preferencesWidgets
    ./streamingWidgets
    ./toolWidgets
    ./utilities
    ./pluginFrameworkWidgets
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

cx_remove_duplicate_include_directories()
add_library( cxGui ${cxGui_SRCS} ${cxResources})
target_include_directories(cxGui
    PUBLIC
    ${SUB_FOLDERS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/resource/videoServer
    ${CustusX_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
    ${CustusX_SOURCE_DIR}/source/logic
    ${CustusX_SOURCE_DIR}/source/modules/GUIExtender
    ${CustusX_SOURCE_DIR}/source/resource/pluginUtilities
    ${CustusX_BINARY_DIR}/source/resource/widgets
    ${CustusX_BINARY_DIR}/source/resource/visualization
    ${CustusX_BINARY_DIR}/source/resource/usReconstructionTypes
    ${CustusX_BINARY_DIR}/source/resource/videoServer
    ${CustusX_BINARY_DIR}/source/resource/OpenIGTLinkUtilities
    ${CustusX_BINARY_DIR}/source/logic
    ${CustusX_BINARY_DIR}/source/modules/GUIExtender
    ${CustusX_BINARY_DIR}/source/resource/pluginUtilities
    )
target_link_libraries(cxGui cxLogicManager cxResourceWidgets)

add_subdirectory(testing)

