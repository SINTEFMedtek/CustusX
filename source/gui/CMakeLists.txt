###########################################################
##               Gui library
###########################################################

cx_add_class_qt_moc(cxGui_SRCS
	browser/cxTreeNode
	browser/cxTreeRepository
	browser/cxTreeItemModel
	browser/cxDataTreeNode
	browser/cxSpaceTreeNode
	browser/cxGroupTreeNode
	browser/cxTopTreeNode
	browser/cxToolTreeNode
	browser/cxTreeNodeImpl
	browser/cxControllableSplitter
	browser/cxShowDataTreeNode
	browser/cxViewServiceTreeNode
	browser/cxViewGroupTreeNode
	browser/cxViewGroupPropertiesWidget
		filterWidgets/cxFilterSetupWidget
		filterWidgets/cxFiltersWidget
	#----------------------
	properties/cxDataInterface
	properties/cxShadingParamsInterfaces
	properties/cxStringPropertyClipPlane
    #----------------------
    dataWidgets/cxVolumePropertiesWidget
    dataWidgets/cxVolumeInfoWidget
    dataWidgets/cxShadingWidget
    dataWidgets/cxCroppingWidget
    dataWidgets/cxClippingWidget
    dataWidgets/cxManageClippersWidget
    dataWidgets/cxSelectClippersForDataWidget
    dataWidgets/cxClipperWidget
    dataWidgets/cxSlicePropertiesWidget
    dataWidgets/cxMeshInfoWidget
	dataWidgets/cxMeshGlyphsWidget
	dataWidgets/cxMeshPropertiesWidget
	dataWidgets/cxImportDataDialog
    dataWidgets/cxMeshTextureWidget
    dataWidgets/cxExportDataDialog
    dataWidgets/cxDataViewSelectionWidget
    dataWidgets/cxTransferFunctionWidget
    dataWidgets/cxTransferFunctionAlphaWidget
    dataWidgets/cxTransferFunctionColorWidget
    dataWidgets/cxEraserWidget
    dataWidgets/cxInfoWidget
    dataWidgets/cxTransferFunctionPresetWidget
    dataWidgets/cxOverlayWidget
    dataWidgets/cxColorWidget
    dataWidgets/cxTransferFunction2DColorWidget
    dataWidgets/cxTransferFunction2DOpacityWidget
    dataWidgets/cxImagePropertiesWidget
    dataWidgets/cxStreamPropertiesWidget
	dataWidgets/cxImagePropertiesWidget
    #----------------------
    infoWidgets/cxStatusBar
    infoWidgets/cxMetricWidget
    infoWidgets/cxDataMetricWrappers
    infoWidgets/cxSamplerWidget
    infoWidgets/cxFrameMetricWrapper
    infoWidgets/cxToolMetricWrapper
    infoWidgets/cxMetricManager
    infoWidgets/cxBrowserWidget

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
	preferencesWidgets/cxOperatingTableWidget
	preferencesWidgets/cxOperatingTableTab
    #----------------------
    streamingWidgets/cxVideoConnectionWidget
    #----------------------
    toolWidgets/cxToolPropertiesWidget
    toolWidgets/cxToolManagerWidget
    #----------------------
    utilities/cxAudioImpl
    utilities/cxToolImagePreviewWidget
    utilities/cxDynamicMainWindowWidgets
    utilities/cxClippers
    utilities/cxMetricUtilities
	#----------------------
    pluginFrameworkWidgets/cxPluginBrowser
    pluginFrameworkWidgets/cxPluginTableModel
    pluginFrameworkWidgets/cxPluginFrameworkWidget
    #----------------------
    cxMainWindow
	cxMainWindowActions
	cxSecondaryMainWindow
    cxSecondaryViewLayoutWindow
    cxLayoutInteractor
	#----------------------
)

cx_add_class(cxGui_SRCS
	${cxGui_SRCS}
	browser/cxReplacableContentWidget
	browser/cxViewServicePropertiesWidget
	cxMainWindowApplicationComponent.h
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
	./filterWidgets
	./properties
    ./dataWidgets
    ./infoWidgets
    ./navigationWidgets
    ./preferencesWidgets
    ./streamingWidgets
    ./toolWidgets
    ./utilities
	./browser
	./pluginFrameworkWidgets
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CustusX_SOURCE_DIR}/source/ThirdParty
)

set(VTK_MODULES
  VTK::InteractionWidgets
)

target_link_libraries(cxGui
    PRIVATE
    cxResource
    cxLogicManager
    cxResourceWidgets
    cxResourceVisualization
    cxGUIExtenderService
    cxPluginUtilities
    cxResourceFilter
    ${VTK_MODULES}
)

vtk_module_autoinit(
  TARGETS cxGui
  MODULES ${VTK_MODULES}
)

cx_doc_define_user_docs("${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/preferences.md")
cx_add_non_source_file("doc/cx_user_doc_widgets_mesh_properties.md")

add_subdirectory(testing)

