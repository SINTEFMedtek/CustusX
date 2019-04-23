###########################################################
##               Resource Core library
###########################################################

set(QXT_FOLDER ${CustusX_SOURCE_DIR}/source/ThirdParty/qxt)

cx_add_class_qt_moc(CX_RESOURCE_WIDGETS_FILES
    cxOptimizedUpdateWidget
	cxConsoleWidgetCollection
	cxConsoleWidget
    cxDoubleWidgets
    cxLabeledComboBoxWidget
    cxDetailedLabeledComboBoxWidget
    cxFilenameWidget
    cxLabeledLineEditWidget
    cxCheckBoxWidget
    cxHelperWidgets
    cxFileSelectWidget
    cxMousePadWidget
    cxColorSelectWidget
    cxColorSelectButton
    cxDisplayTimerWidget
    cxTimelineWidget
    cxDoubleSpanSlider
    cxBaseWidget
    cxTransform3DWidget
    cxSelectionGroupBox
    cxVector3DWidget
    cxBoundingBoxWidget
    cxFileWatcherWidget
    cxFilePreviewWidget
    cxImagePreviewWidget
    cxPresetWidget
    cxFileInputWidget
    cxMultiFileInputWidget
    cxFilterPresetWidget
    cxTabbedWidget
    cxSpaceEditWidget
    cxWidgetObscuredListener
	cxOptionsWidget
	cxDataSelectWidget
    cxTimedAlgorithmProgressBar
	cxProbeConfigWidget
	cxFrameTreeWidget
    cxPipelineWidget
    cxCompactFilterSetupWidget
	cxActiveToolWidget
	cxPopupToolbarWidget
	cxStringListSelectWidget
	cxWebServerWidgetBase
	${QXT_FOLDER}/qxtspanslider_p.h
    ${QXT_FOLDER}/qxtspanslider
)

cx_add_class(CX_RESOURCE_WIDGETS_FILES
    snwSyntaxHighlighter
    )

cx_create_export_header("cxResourceWidgets")

add_library( cxResourceWidgets ${CX_RESOURCE_WIDGETS_FILES})
target_include_directories(cxResourceWidgets
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/source/ThirdParty/qxt
)
target_link_libraries(cxResourceWidgets
    PRIVATE
    cxResource
    cxResourceFilter
	cxResourceVisualization
)

add_subdirectory(testing)
