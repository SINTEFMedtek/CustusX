cx_add_class_qt_moc(GUIExtender_FILES
    cxGUIExtenderService
)

cx_add_class(GUIExtender_FILES
)

cx_create_export_header("cxGUIExtenderService")

add_library( cxGUIExtenderService ${GUIExtender_FILES})
target_include_directories(cxGUIExtenderService
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(cxGUIExtenderService
    PRIVATE
    Qt5::Core
    ${SSC_GCOV_LIBRARY}
)
