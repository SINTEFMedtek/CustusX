#
# Build the USER documentation
#
find_package(Doxygen)

if(Doxygen_FOUND)
    option(CX_BUILD_USER_DOCUMENTATION "Build the user documentation and embed in installer" ON)
else()
    option(CX_BUILD_USER_DOCUMENTATION "Build the user documentation and embed in installer" OFF)
endif()

IF (CX_BUILD_USER_DOCUMENTATION)
    SET(CX_DOC_INPUT_DIRECTORY ${CustusX_SOURCE_DIR}/documentation)
    SET(CX_DOC_OUTPUT_DIRECTORY ${CustusX_BINARY_DIR}/documentation)

    SET(CX_DOXYGEN_USE_WARNINGS YES)
    SET(CX_DOXYGEN_USE_SHORT_NAMES NO)
    SET(CX_DOXYGEN_HAVE_DOT_YESNO NO)
    
    # =====================================================
    # generate qt qhp html doc
    # =====================================================
    SET(CX_DOXYGEN_HTML_OUTPUT html_qhp)
    SET(CX_DOXYGEN_OUTPUT_DIRECTORY ${CX_DOC_OUTPUT_DIRECTORY})
    SET(CX_DOXYGEN_PROJECT_LOGO ${CustusX_SOURCE_DIR}/source/gui/icons/CustusX_64x64.png)
    SET(CX_DOXYGEN_INPUT ${CX_DOC_INPUT_DIRECTORY})
    SET(CX_DOXYGEN_EXCLUDE )
    SET(CX_DOXYGEN_EXCLUDE_PATTERNS */.git/* */testing/*)   
    SET(CX_DOXYGEN_IMAGE_PATH "${CustusX_SOURCE_DIR}/doc/doxygen/images ${CustusX_SOURCE_DIR}/documentation/images")
    SET(CX_DOXYGEN_HTML_EXTRA_FILES ${CustusX_SOURCE_DIR}/doc/cx_code_standard.pdf)
    SET(CX_DOXYGEN_GENERATE_QHP YES)
#    SET(CX_DOXYGEN_QCH_FILE ../cx_user_doc.qch) # relative to html dir
    SET(CX_DOXYGEN_QHP_NAMESPACE org.custusx.core)
#    SET(CX_DOXYGEN_QHG_LOCATION ${QT_QCOLLECTIONGENERATOR_EXECUTABLE}) # check if this is the correct help generator.
#    SET(CX_DOXYGEN_QHG_LOCATION "qhelpgenerator") # check if this is the correct help generator.
     
     # tweaks that make pages more fitting in the internal help system
    SET(CX_DOXYGEN_HTML_HEADER ${CX_DOC_INPUT_DIRECTORY}/doxy_header_empty.html)
    SET(CX_DOXYGEN_HTML_FOOTER ${CX_DOC_INPUT_DIRECTORY}/doxy_footer_empty.html)
    SET(CX_DOXYGEN_DISABLE_INDEX YES)
    SET(CX_DOXYGEN_GENERATE_TREEVIEW NO)
    SET(CX_DOXYGEN_SEARCHENGINE NO)
    SET(CX_DOXYGEN_ALPHABETICAL_INDEX NO)
    SET(CX_DOXYGEN_SOURCE_BROWSER NO)
    SET(CX_DOXYGEN_EXTRACT_ALL NO)
    SET(CX_DOXYGEN_BUILTIN_STL_SUPPORT NO) # when NO this one disables lots of std::stuff in the TOC.

    CONFIGURE_FILE(
        ${CX_DOC_INPUT_DIRECTORY}/cx_doxyfile.dox.in
        ${CX_DOC_OUTPUT_DIRECTORY}/cx_doxyfile_qhp.dox)
    CONFIGURE_FILE(
        ${CX_DOC_INPUT_DIRECTORY}/cx_user_doc.qhcp.in
        ${CX_DOC_OUTPUT_DIRECTORY}/cx_user_doc.qhcp)


    ADD_CUSTOM_TARGET(UserDoc
        ALL
        ${DOXYGEN_EXECUTABLE}
        ${CX_DOC_OUTPUT_DIRECTORY}/cx_doxyfile_qhp.dox)

    ADD_DEPENDENCIES(UserDoc CustusX)

    find_package(Qt5Help)
    get_target_property(CX_QT_QCOLLECTIONGENERATOR_EXECUTABLE Qt5::qcollectiongenerator LOCATION)
#    get_target_property(CX_QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION)
#    message(STATUS "CX_QT_QMAKE_EXECUTABLE " ${CX_QT_QMAKE_EXECUTABLE})

    add_custom_command(TARGET UserDoc
               POST_BUILD
               COMMAND ${CX_QT_QCOLLECTIONGENERATOR_EXECUTABLE}
               	       ${CX_DOC_OUTPUT_DIRECTORY}/cx_user_doc.qhcp 
                       -o ${CX_DOC_OUTPUT_DIRECTORY}/cx_user_doc.qhc
               WORKING_DIRECTORY ${CX_DOC_OUTPUT_DIRECTORY}
               COMMENT "Generating Qt Help Collection ${CX_DOC_OUTPUT_DIRECTORY}/cx_user_doc.qhc " 
               VERBATIM)

    # =====================================================
    # generate pure html doc
    # =====================================================
    SET(CX_DOXYGEN_GENERATE_QHP NO)
    SET(CX_DOXYGEN_HTML_OUTPUT html_pure)
    SET(CX_DOXYGEN_DISABLE_INDEX NO)
    SET(CX_DOXYGEN_GENERATE_TREEVIEW YES)
    SET(CX_DOXYGEN_SEARCHENGINE YES)
    SET(CX_DOXYGEN_ALPHABETICAL_INDEX YES)
    SET(CX_DOXYGEN_HTML_HEADER "")
    SET(CX_DOXYGEN_HTML_FOOTER "")
    
    CONFIGURE_FILE(
        ${CX_DOC_INPUT_DIRECTORY}/cx_doxyfile.dox.in
        ${CX_DOC_OUTPUT_DIRECTORY}/cx_doxyfile_html.dox)
    add_custom_command(TARGET UserDoc
        POST_BUILD
        COMMAND ${DOXYGEN_EXECUTABLE}
            ${CX_DOC_OUTPUT_DIRECTORY}/cx_doxyfile_html.dox
        WORKING_DIRECTORY ${CX_DOC_OUTPUT_DIRECTORY}
        COMMENT "Generating pure html ${CX_DOC_OUTPUT_DIRECTORY}/html_pure" 
        VERBATIM)
    ADD_DEPENDENCIES(UserDoc CustusX)

ENDIF ()
