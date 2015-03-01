#
# Build the USER documentation
#
find_package(Doxygen)

INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)

MARK_AS_ADVANCED(CLEAR BUILD_DOCUMENTATION) # CLEAR means not advanced

if(Doxygen_FOUND)
    option(CX_BUILD_USER_DOCUMENTATION "Build the user documentation and embed in installer" ON)
else()
    option(CX_BUILD_USER_DOCUMENTATION "Build the user documentation and embed in installer" OFF)
endif()

###############################################################################
#
# Generate CustusX user documentation on Qt QHP format.
#
###############################################################################
function(cx_build_user_documentation_qt_help)
    message(STATUS " **** Generating user qthelp documentation...")

    SET(CX_DOC_INPUT_DIRECTORY ${CustusX_SOURCE_DIR}/doc)
    SET(CX_DOC_OUTPUT_DIRECTORY ${CustusX_BINARY_DIR}/doc)

    SET(CX_DOXYGEN_USE_WARNINGS YES)
    SET(CX_DOXYGEN_USE_SHORT_NAMES NO)
    SET(CX_DOXYGEN_HAVE_DOT_YESNO NO)
    
    SET(CX_DOXYGEN_HTML_OUTPUT html_qhp)
    SET(CX_DOXYGEN_OUTPUT_DIRECTORY ${CX_DOC_OUTPUT_DIRECTORY})
    SET(CX_DOXYGEN_PROJECT_LOGO ${CustusX_SOURCE_DIR}/source/gui/icons/CustusX_64x64.png)
    SET(CX_DOXYGEN_INPUT "${CX_DOC_INPUT_DIRECTORY}/user_manual ${CX_DOC_INPUT_DIRECTORY}/shared_manual")
    SET(CX_DOXYGEN_EXCLUDE )
    SET(CX_DOXYGEN_EXCLUDE_PATTERNS "*/.git/* */testing/*")   
    SET(CX_DOXYGEN_IMAGE_PATH "${CustusX_SOURCE_DIR}/doc/images")
    SET(CX_DOXYGEN_HTML_EXTRA_FILES )
    SET(CX_DOXYGEN_GENERATE_QHP YES)
    SET(CX_DOXYGEN_QHP_NAMESPACE org.custusx.core)
    set(CX_USE_MDFILE_AS_MAINPAGE {CX_DOC_INPUT_DIRECTORY}/user_manual/cx_user_doc_main_page.md)
     
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

	ADD_DEPENDENCIES(UserDoc ${CX_SYSTEM_DEFAULT_APPLICATION})

    find_package(Qt5Help)
    # works on mac
    get_target_property(CX_QT_QCOLLECTIONGENERATOR_EXECUTABLE Qt5::qcollectiongenerator LOCATION)
    if (NOT TARGET Qt5::qcollectiongenerator)
      # required on ubuntu
      set(CX_QT_QCOLLECTIONGENERATOR_EXECUTABLE qcollectiongenerator)
    endif()
    # .. not tested on win

    #get_target_property(CX_QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION)
    #message(STATUS "CX_QT_QCOLLECTIONGENERATOR_EXECUTABLE " ${CX_QT_QCOLLECTIONGENERATOR_EXECUTABLE})

    add_custom_command(TARGET UserDoc
               POST_BUILD
               COMMAND ${CX_QT_QCOLLECTIONGENERATOR_EXECUTABLE}
               	       ${CX_DOC_OUTPUT_DIRECTORY}/cx_user_doc.qhcp 
                       -o ${CX_DOC_OUTPUT_DIRECTORY}/cx_user_doc.qhc
               WORKING_DIRECTORY ${CX_DOC_OUTPUT_DIRECTORY}
               COMMENT "Generating Qt Help Collection ${CX_DOC_OUTPUT_DIRECTORY}/cx_user_doc.qhc " 
               VERBATIM)
endfunction()

###############################################################################
#
# Generate CustusX user documentation on HTML format.
#
###############################################################################
function(cx_build_user_documentation_html)
    message(STATUS " **** Generating user html documentation...")

    SET(CX_DOC_INPUT_DIRECTORY ${CustusX_SOURCE_DIR}/doc)
    SET(CX_DOC_OUTPUT_DIRECTORY ${CustusX_BINARY_DIR}/doc)

    SET(CX_DOXYGEN_USE_WARNINGS YES)
    SET(CX_DOXYGEN_USE_SHORT_NAMES NO)
    SET(CX_DOXYGEN_HAVE_DOT_YESNO NO)
    
    SET(CX_DOXYGEN_OUTPUT_DIRECTORY ${CX_DOC_OUTPUT_DIRECTORY})
    SET(CX_DOXYGEN_PROJECT_LOGO ${CustusX_SOURCE_DIR}/source/gui/icons/CustusX_64x64.png)
    SET(CX_DOXYGEN_INPUT "${CX_DOC_INPUT_DIRECTORY}/user_manual ${CX_DOC_INPUT_DIRECTORY}/shared_manual")
    SET(CX_DOXYGEN_EXCLUDE )
    SET(CX_DOXYGEN_EXCLUDE_PATTERNS "*/.git/* */testing/*")   
    SET(CX_DOXYGEN_IMAGE_PATH "${CustusX_SOURCE_DIR}/doc/images")
    SET(CX_DOXYGEN_HTML_EXTRA_FILES )
     
    # tweaks that make pages more fitting in the internal help system
    SET(CX_DOXYGEN_SOURCE_BROWSER NO)
    SET(CX_DOXYGEN_EXTRACT_ALL NO)
    SET(CX_DOXYGEN_BUILTIN_STL_SUPPORT NO) # when NO this one disables lots of std::stuff in the TOC.
    # end tweaks

    SET(CX_DOXYGEN_DISABLE_INDEX NO)
    SET(CX_DOXYGEN_GENERATE_TREEVIEW YES)
    SET(CX_DOXYGEN_SEARCHENGINE YES)
    SET(CX_DOXYGEN_ALPHABETICAL_INDEX YES)

    SET(CX_DOXYGEN_GENERATE_QHP NO)
#	SET(CX_DOXYGEN_INPUT "${CX_DOC_INPUT_DIRECTORY} ${CustusX_SOURCE_DIR}/doc/doxygen")
    SET(CX_DOXYGEN_HTML_OUTPUT html_pure)
    set(CX_USE_MDFILE_AS_MAINPAGE {CX_DOC_INPUT_DIRECTORY}/user_manual/cx_user_doc_main_page.md)
    
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
	ADD_DEPENDENCIES(UserDoc ${CX_SYSTEM_DEFAULT_APPLICATION})
endfunction()

###############################################################################
#
# Generate CustusX developer documentation on html format
#
###############################################################################
function(cx_build_dev_documentation_html)
    message(STATUS " **** Generating developer html documentation...")

    SET(CX_DOC_INPUT_DIRECTORY ${CustusX_SOURCE_DIR}/doc)
    SET(CX_DOC_OUTPUT_DIRECTORY ${CustusX_BINARY_DIR}/doc)

    SET(CX_DOXYGEN_USE_WARNINGS YES)
    SET(CX_DOXYGEN_USE_SHORT_NAMES NO)
    SET(CX_DOXYGEN_HAVE_DOT_YESNO NO) # ?????
    
  SET(CX_DOXYGEN_HAVE_DOT_YESNO NO)
  IF(DOT)
    SET(CX_DOXYGEN_HAVE_DOT_YESNO YES)
    IF(NOT DOT_PATH)
      GET_FILENAME_COMPONENT(DOT_PATH ${DOT} PATH)
    ENDIF(NOT DOT_PATH)
  ENDIF(DOT)
    
    
    SET(CX_DOXYGEN_OUTPUT_DIRECTORY ${CX_DOC_OUTPUT_DIRECTORY})
    SET(CX_DOXYGEN_PROJECT_LOGO ${CustusX_SOURCE_DIR}/source/gui/icons/CustusX_64x64.png)
#    SET(CX_DOXYGEN_INPUT "${CX_DOC_INPUT_DIRECTORY}/dev_manual")
    SET(CX_DOXYGEN_INPUT "${CX_DOC_INPUT_DIRECTORY}/dev_manual ${CX_DOC_INPUT_DIRECTORY}/shared_manual ${CustusX_SOURCE_DIR}/source ${CustusX_SOURCE_DIR}/CMake")
    SET(CX_DOXYGEN_EXCLUDE )
    SET(CX_DOXYGEN_EXCLUDE_PATTERNS "*/.git/* */testing/* */ThirdParty/*")   
    SET(CX_DOXYGEN_IMAGE_PATH "${CustusX_SOURCE_DIR}/doc/images")
    SET(CX_DOXYGEN_HTML_EXTRA_FILES )
     
    # tweaks that make pages more fitting in the internal help system
    SET(CX_DOXYGEN_SOURCE_BROWSER YES)
    SET(CX_DOXYGEN_EXTRACT_ALL YES)
    SET(CX_DOXYGEN_BUILTIN_STL_SUPPORT NO) # when NO this one disables lots of std::stuff in the TOC.
    # end tweaks

    SET(CX_DOXYGEN_DISABLE_INDEX NO)
    SET(CX_DOXYGEN_GENERATE_TREEVIEW YES)
    SET(CX_DOXYGEN_SEARCHENGINE YES)
    SET(CX_DOXYGEN_ALPHABETICAL_INDEX YES)

    SET(CX_DOXYGEN_GENERATE_QHP NO)
#	SET(CX_DOXYGEN_INPUT "${CX_DOC_INPUT_DIRECTORY} ${CustusX_SOURCE_DIR}/doc/doxygen")
    SET(CX_DOXYGEN_HTML_OUTPUT html_dev)
    set(CX_USE_MDFILE_AS_MAINPAGE {CX_DOC_INPUT_DIRECTORY}/dev_manual/cx_dev_main_page.md)
    
    
    ADD_CUSTOM_TARGET(DoxygenDoc
        ALL
        ${DOXYGEN_EXECUTABLE}
        ${CX_DOC_OUTPUT_DIRECTORY}/cx_doxyfile_dev_html.dox)
    
    CONFIGURE_FILE(
        ${CX_DOC_INPUT_DIRECTORY}/cx_doxyfile.dox.in
        ${CX_DOC_OUTPUT_DIRECTORY}/cx_doxyfile_dev_html.dox)
    add_custom_command(TARGET DoxygenDoc
        POST_BUILD
        COMMAND ${DOXYGEN_EXECUTABLE}
            ${CX_DOC_OUTPUT_DIRECTORY}/cx_doxyfile_dev_html.dox
        WORKING_DIRECTORY ${CX_DOC_OUTPUT_DIRECTORY}
        COMMENT "Generating dev html ${CX_DOC_OUTPUT_DIRECTORY}/html_dev" 
        VERBATIM)
	ADD_DEPENDENCIES(DoxygenDoc ${CX_SYSTEM_DEFAULT_APPLICATION})
endfunction()


###############################################################################
#
# Generate CustusX documentation
#
###############################################################################
IF (BUILD_DOCUMENTATION)
    cx_build_dev_documentation_html()
ENDIF (BUILD_DOCUMENTATION)

if(CX_BUILD_USER_DOCUMENTATION)
    cx_build_user_documentation_qt_help()
    cx_build_user_documentation_html()
endif()
