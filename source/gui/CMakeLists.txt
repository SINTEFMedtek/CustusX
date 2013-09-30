###########################################################
##               Gui library
###########################################################


cx_add_class_qt_moc(cxGui_SRCS
	infoWidgets/cxMetricManager
	#----------------------
	dataAdapters/cxDataInterface
	dataAdapters/cxShadingParamsInterfaces
	dataAdapters/cxToolDataAdapters
	dataAdapters/cxDoubleDataAdapterTemporalCalibration
	dataAdapters/cxDataAdapterHelper
	dataAdapters/cxSelectDataStringDataAdapter
    #----------------------
	dataWidgets/cxVolumePropertiesWidget
	dataWidgets/cxVolumeInfoWidget
	dataWidgets/cxShadingWidget
	dataWidgets/cxCroppingWidget
	dataWidgets/cxClippingWidget
	dataWidgets/cxImagePropertiesWidget
	dataWidgets/cxMeshInfoWidget
	dataWidgets/cxImportDataDialog
	dataWidgets/cxExportDataDialog
	dataWidgets/cxDataViewSelectionWidget
	dataWidgets/cxTransferFunctionWidget
	dataWidgets/cxTransferFunctionAlphaWidget
	dataWidgets/cxTransferFunctionColorWidget
#	dataWidgets/sscDICOMWidget
	dataWidgets/cxEraserWidget
	dataWidgets/cxDataSelectWidget
	dataWidgets/cxInfoWidget
	dataWidgets/cxTransferFunctionPresetWidget
	dataWidgets/cxOverlayWidget
	dataWidgets/cxColorWidget
	dataWidgets/cxTransferFunction2DColorWidget
	dataWidgets/cxTransferFunction2DOpacityWidget
    #----------------------
	infoWidgets/cxStatusBar
	infoWidgets/cxPointSamplingWidget
	infoWidgets/cxMetricWidget
	infoWidgets/cxFrameTreeWidget
	infoWidgets/cxDataMetricWrappers
	infoWidgets/cxSamplerWidget
	infoWidgets/cxFrameMetricWrapper
        infoWidgets/cxToolMetricWrapper
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
	preferencesWidgets/cxLayoutEditorTab
    #----------------------
	streamingWidgets/cxVideoConnectionWidget
	streamingWidgets/cxSimulateUSWidget
    #----------------------
	toolWidgets/cxToolPropertiesWidget
	toolWidgets/cxActiveToolWidget
	toolWidgets/cxToolManagerWidget
	toolWidgets/cxProbeConfigWidget
    #----------------------
	utilities/cxAudio
	utilities/cxToolImagePreviewWidget
    #----------------------    
	cxMainWindow
	cxSecondaryMainWindow
    #----------------------
)


# generate rules for building source files from the resources
QT4_ADD_RESOURCES(cxGui_SRCS cxResources.qrc)

cx_add_class(cxGui_SRCS
	${cxGui_SRCS}
	${cxGui_RCC_SRCS}
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
    )

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${SUB_FOLDERS}
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)
include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ${CustusX3_SOURCE_DIR}/source/resource/videoServer
    ${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
    ${CustusX3_SOURCE_DIR}/source/service/patient     
    ${CustusX3_SOURCE_DIR}/source/service/tracking     
    ${CustusX3_SOURCE_DIR}/source/service/visualization
    ${CustusX3_SOURCE_DIR}/source/service/video
    ${CustusX3_SOURCE_DIR}/source/service/state
    
    ${CustusX3_SOURCE_DIR}/source/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    )
    
set(CX_GUI_LIBRARIES cxLogicManager cxStateService cxResourceWidgets)
if (SSC_USE_DCMTK)
    set(CX_GUI_LIBRARIES ${CX_GUI_LIBRARIES} DICOMLib)
endif (SSC_USE_DCMTK)
    
add_library( cxGui ${cxGui_SRCS} )
target_link_libraries( cxGui ${CX_GUI_LIBRARIES} )
CX_COTIRE_ADD_LIBRARY(cxGui "${CX_GUI_LIBRARIES}")

add_subdirectory(testing)

