###########################################################
##               Resource Core library
###########################################################

set(QXT_FOLDER ${CustusX_SOURCE_DIR}/source/ThirdParty/qxt)

cx_add_class_qt_moc(CX_RESOURCE_WIDGETS_FILES
	cxOptimizedUpdateWidget
	cxConsoleWidget
	cxDoubleWidgets
	cxLabeledComboBoxWidget
	cxDetailedLabeledComboBoxWidget
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
	${QXT_FOLDER}/qxtspanslider_p.h
	${QXT_FOLDER}/qxtspanslider
	)

cx_add_class(CX_RESOURCE_WIDGETS_FILES
	cxTabbedWidget
	snwSyntaxHighlighter
    )

unset(CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES)
set(CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/resource/widgets
    ${QXT_FOLDER}
    CACHE INTERNAL "Include directories for resource widgets library")
include_directories(${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES} )

set(CX_RESOURCE_WIDGETS_LIBRARIES
    ${CX_RESOURCE_WIDGETS_LIBRARIES}
    cxResource
    )

add_library( cxResourceWidgets ${CX_RESOURCE_WIDGETS_FILES})
target_link_libraries(cxResourceWidgets ${CX_RESOURCE_WIDGETS_LIBRARIES} )

#add_subdirectory(testing)
