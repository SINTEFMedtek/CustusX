
cx_add_class_qt_moc(GUIExtender_FILES
    cxGUIExtenderService
	)

cx_add_class(GUIExtender_FILES
    )

set(GUIExtender_LIBRARIES
    Qt5::Core
    )

add_library( cxGUIExtenderService ${GUIExtender_FILES})
target_link_libraries(cxGUIExtenderService ${GUIExtender_LIBRARIES} ${SSC_GCOV_LIBRARY})
