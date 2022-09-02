###############################################################################
# Build the USER and DEVELOPER documentation
###############################################################################

if(CX_BUILD_USER_DOCUMENTATION)
	find_package( Doxygen 1.8.6 REQUIRED)
else()
	find_package( Doxygen 1.8.6 QUIET)
endif()




INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)

MARK_AS_ADVANCED(CLEAR BUILD_DOCUMENTATION) # CLEAR means not advanced

if(Doxygen_FOUND)
    option(CX_BUILD_USER_DOCUMENTATION "Build the user documentation and embed in installer" ON)
else()
    option(CX_BUILD_USER_DOCUMENTATION "Build the user documentation and embed in installer" OFF)
endif()

###############################################################################
#
# Install all documentation files for CustusX
#
# The qt help collection files are sufficient for in-app help.
# Other help is available on the web site.
#
###############################################################################
function(cx_install_documentation_files)
	install(FILES
			${CustusX_BINARY_DIR}/doc/cx_user_doc.qhc
			${CustusX_BINARY_DIR}/doc/cx_user_doc.qch
                        DESTINATION ${CX_INSTALL_DOC_DIR}/)
endfunction()

###############################################################################
#
# Generate plugin page cx_user_doc_plugins.md 
# based on info about active plugins.
#
###############################################################################
function(cx_doc_generate_plugin_page)
    get_property(PLUGIN_DOC_NAMES GLOBAL PROPERTY CX_PLUGIN_DOC_NAMES)    

    # Generate PLUGIN_SUBPAGES_TEXT containing \subpage refs to all documented plugins
    # ready to insert into markdown.
    foreach(_var ${PLUGIN_DOC_NAMES})
        string(REPLACE "_" "." plugin_name ${_var})
        string(TOLOWER ${_var} plugin_index)
        set(PLUGIN_SUBPAGES_TEXT "${PLUGIN_SUBPAGES_TEXT}\n* \\subpage ${plugin_index} (${plugin_name})" )
    endforeach()
    #message(STATUS "PLUGIN_SUBPAGES_TEXT : " ${PLUGIN_SUBPAGES_TEXT})

    # Generate ALL_ACTIVE_PLUGIN_NAMES containing all plugins found in the system
    getListOfVarsStartingWith(CX_PLUGIN_ ALL_PLUGINS)
    #message(STATUS "ALL_PLUGINS: " )
    foreach(_var ${ALL_PLUGINS})
        if(${_var})
            #message(STATUS "ON    " "${_var}" )
            string(REGEX REPLACE "CX_PLUGIN_(.+):.+" "\\1" plugin_name ${_var})
            string(REPLACE "CX_PLUGIN_" "" plugin_name ${_var})
            string(REPLACE "." "_" plugin_name ${plugin_name})
            #message(STATUS "  plugin_name: " ${plugin_name})
            set(ALL_ACTIVE_PLUGIN_NAMES ${ALL_ACTIVE_PLUGIN_NAMES} ${plugin_name})
         else()
            #message(STATUS "OFF    " "${_var}" )
         endif()
    endforeach()

    # Generate UNDOCUMENTED_PLUGIN_NAMES containing all plugins not documented
    foreach(_var ${ALL_ACTIVE_PLUGIN_NAMES})
        list(FIND PLUGIN_DOC_NAMES ${_var} FOUND_IN_DOCS)
            if(${FOUND_IN_DOCS} EQUAL -1)
                set(UNDOCUMENTED_PLUGIN_NAMES ${UNDOCUMENTED_PLUGIN_NAMES} ${_var})               
            endif()
    endforeach()

    #message(STATUS "UNDOCUMENTED_PLUGIN_NAMES: " )
    #foreach(_var ${UNDOCUMENTED_PLUGIN_NAMES})
    #    message(STATUS UNDOCUMENTED_PLUGIN_NAMES "${_var}" )
    #endforeach()

    # Generate UNDOCUMENTED_PLUGINS_TEXT containing \list of all undocumented plugins,
    # ready to insert into markdown.
    foreach(_var ${UNDOCUMENTED_PLUGIN_NAMES})
        string(REPLACE "_" "." plugin_name ${_var})
        set(UNDOCUMENTED_PLUGINS_TEXT "${UNDOCUMENTED_PLUGINS_TEXT}\n* ${plugin_name}" )
    endforeach()
    #message(STATUS "UNDOCUMENTED_PLUGINS_TEXT : " ${UNDOCUMENTED_PLUGINS_TEXT})

    CONFIGURE_FILE(
        ${CX_DOC_INPUT_DIRECTORY}/shared_manual/cx_user_doc_plugins.md.in
        ${CX_DOC_OUTPUT_DIRECTORY}/cx_user_doc_plugins.md)
    #PLUGIN_SUBPAGES_TEXT
endfunction()


###############################################################################
#
# Set common defines
# for user doc.
#
###############################################################################
macro(cx_doc_set_common_userdoc_definitions)

    SET(CX_DOC_INPUT_DIRECTORY ${CustusX_SOURCE_DIR}/doc)
    SET(CX_DOC_OUTPUT_DIRECTORY ${CustusX_BINARY_DIR}/doc)

    SET(CX_DOXYGEN_USE_WARNINGS YES)
    SET(CX_DOXYGEN_USE_SHORT_NAMES NO)
    SET(CX_DOXYGEN_HAVE_DOT_YESNO NO)
    
    SET(CX_DOXYGEN_OUTPUT_DIRECTORY ${CX_DOC_OUTPUT_DIRECTORY})
    SET(CX_DOXYGEN_PROJECT_LOGO ${CustusX_SOURCE_DIR}/source/gui/icons/CustusX/CustusX_64x64.png)

    get_property(PLUGIN_DOC_FOLDERS GLOBAL PROPERTY CX_PLUGIN_DOC_FOLDERS)

    # set image folders
    SET(CX_DOXYGEN_IMAGE_PATH 
        ${CX_DOC_INPUT_DIRECTORY}/images 
        ${PLUGIN_DOC_FOLDERS})
    TO_LIST_SPACES(CX_DOXYGEN_IMAGE_PATH CX_DOXYGEN_IMAGE_PATH)

    # set input files
    SET(CX_DOXYGEN_INPUT 
        ${CX_DOC_INPUT_DIRECTORY}/user_manual 
        ${CX_DOC_INPUT_DIRECTORY}/shared_manual
        ${CX_DOC_OUTPUT_DIRECTORY}/cx_user_doc_plugins.md
        ${PLUGIN_DOC_FOLDERS})
    TO_LIST_SPACES(CX_DOXYGEN_INPUT CX_DOXYGEN_INPUT)

    cx_doc_generate_plugin_page()

    SET(CX_DOXYGEN_EXCLUDE )
    SET(CX_DOXYGEN_EXCLUDE_PATTERNS "*/.git/* */testing/*")   
    SET(CX_DOXYGEN_HTML_EXTRA_FILES )

    get_property(CX_DOC_USER_MAINPAGE_OVERRIDE GLOBAL PROPERTY CX_DOC_USER_MAINPAGE_OVERRIDE)
    # remove the default mainpage if an override is defined:
    if(CX_DOC_USER_MAINPAGE_OVERRIDE)
        set(CX_DOXYGEN_EXCLUDE ${CX_DOXYGEN_EXCLUDE} "${CX_DOC_INPUT_DIRECTORY}/user_manual/cx_user_doc_main_page.md")
    endif()
    
    # tweaks that remove source code stuff
    SET(CX_DOXYGEN_SOURCE_BROWSER NO)
    SET(CX_SHOW_FILES NO)
    SET(CX_DOXYGEN_EXTRACT_ALL NO)
    SET(CX_DOXYGEN_BUILTIN_STL_SUPPORT NO) # when NO this one disables lots of std::stuff in the TOC.
    # end tweaks
    
endmacro()


###############################################################################
#
# Generate CustusX user documentation on Qt QHP format.
#
###############################################################################
function(cx_build_user_documentation_qt_help)
    message(STATUS " **** Generating user qthelp documentation...")

    cx_doc_set_common_userdoc_definitions()
    
    SET(CX_DOXYGEN_HTML_OUTPUT html_qhp)    
    SET(CX_DOXYGEN_GENERATE_QHP YES)
    SET(CX_DOXYGEN_QHP_NAMESPACE org.custusx.core)
     
     # tweaks that make pages more fitting in the internal help system
    SET(CX_DOXYGEN_HTML_HEADER ${CX_DOC_INPUT_DIRECTORY}/doxy_header_empty.html)
    SET(CX_DOXYGEN_HTML_FOOTER ${CX_DOC_INPUT_DIRECTORY}/doxy_footer_empty.html)
    SET(CX_DOXYGEN_DISABLE_INDEX YES)
    SET(CX_DOXYGEN_GENERATE_TREEVIEW NO)
    SET(CX_DOXYGEN_SEARCHENGINE NO)
    SET(CX_DOXYGEN_ALPHABETICAL_INDEX NO)
    # end tweaks
    
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

    cx_doc_set_common_userdoc_definitions()

    SET(CX_DOXYGEN_GENERATE_QHP NO)
    SET(CX_DOXYGEN_HTML_OUTPUT html_pure)

    SET(CX_DOXYGEN_DISABLE_INDEX NO)
    SET(CX_DOXYGEN_GENERATE_TREEVIEW YES)
    SET(CX_DOXYGEN_SEARCHENGINE YES)
    SET(CX_DOXYGEN_ALPHABETICAL_INDEX YES)
    
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
    SET(CX_DOXYGEN_PROJECT_LOGO ${CustusX_SOURCE_DIR}/source/gui/icons/CustusX/CustusX_64x64.png)

    SET(CX_DOXYGEN_INPUT 
        ${CX_DOC_INPUT_DIRECTORY}/dev_manual 
        ${CX_DOC_INPUT_DIRECTORY}/shared_manual 
        ${CX_DOC_OUTPUT_DIRECTORY}/cx_user_doc_plugins.md
        ${PLUGIN_DOC_FOLDERS}
        ${CustusX_SOURCE_DIR}/source 
        ${CustusX_SOURCE_DIR}/CMake)
    TO_LIST_SPACES(CX_DOXYGEN_INPUT CX_DOXYGEN_INPUT)
    
    SET(CX_DOXYGEN_EXCLUDE )
    SET(CX_DOXYGEN_EXCLUDE_PATTERNS "*/.git/* */testing/* */ThirdParty/* README.md")
    SET(CX_DOXYGEN_IMAGE_PATH "${CustusX_SOURCE_DIR}/doc/images")
    SET(CX_DOXYGEN_HTML_EXTRA_FILES )
     
    # tweaks that make pages more fitting in the internal help system
    SET(CX_DOXYGEN_SOURCE_BROWSER YES)
    SET(CX_SHOW_FILES YES)
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
	cx_install_documentation_files()
endif()


cx_add_non_source_file("dev_manual/cx_dev_architecture.md")
cx_add_non_source_file("dev_manual/cx_dev_build_instructions.md")
cx_add_non_source_file("dev_manual/cx_dev_code_style.md")
cx_add_non_source_file("dev_manual/cx_dev_customize_applications.md")
cx_add_non_source_file("dev_manual/cx_dev_main_page.md")
cx_add_non_source_file("dev_manual/cx_dev_superbuild.md")
cx_add_non_source_file("dev_manual/cx_dev_group_definitions.h")
cx_add_non_source_file("dev_manual/cx_dev_group_resource.h")

cx_add_non_source_file("user_manual/cx_user_doc_main_page.md")

cx_add_non_source_file("user_manual/tutorial/cx_user_tutorial_customize_user_interface.md")
cx_add_non_source_file("user_manual/tutorial/cx_user_tutorial_dicom_import.md")
cx_add_non_source_file("user_manual/tutorial/cx_user_tutorial_image_landmark_registration.md")
cx_add_non_source_file("user_manual/tutorial/cx_user_tutorial_patient_landmark_registration.md")
cx_add_non_source_file("user_manual/tutorial/cx_user_tutorial_tracking.md")
cx_add_non_source_file("user_manual/tutorial/cx_user_tutorial_ultrasound_acquisition.md")
cx_add_non_source_file("user_manual/tutorial/cx_user_tutorial_video_streaming.md")
cx_add_non_source_file("user_manual/tutorial/cx_user_tutorial_volumes.md")
cx_add_non_source_file("user_manual/tutorial/cx_user_tutorial.md")

cx_add_non_source_file("shared_manual/cx_dev_coordinate_systems.md")
cx_add_non_source_file("shared_manual/cx_external_libraries.md")
cx_add_non_source_file("shared_manual/cx_license.md")
cx_add_non_source_file("shared_manual/cx_user_supported_platforms.md")
cx_add_non_source_file("shared_manual/cx_us_probe_definition.md")

cx_add_non_source_file("shared_manual/features/cx_user_definitions.md")
cx_add_non_source_file("shared_manual/features/cx_user_doc_feature_overview.md")
cx_add_non_source_file("shared_manual/features/cx_user_doc_widgets_algorithms.md")
cx_add_non_source_file("shared_manual/features/cx_user_doc_widgets_browsing.md")
cx_add_non_source_file("shared_manual/features/cx_user_doc_widgets_properties.md")
cx_add_non_source_file("shared_manual/features/cx_user_doc_widgets_utility.md")
cx_add_non_source_file("shared_manual/features/cx_user_group_definitions.h")

cx_add_non_source_file("shared_manual/installation/cx_user_installation.md")
cx_add_non_source_file("shared_manual/installation/cx_user_ndi_install.md")

