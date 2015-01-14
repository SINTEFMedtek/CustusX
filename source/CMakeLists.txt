###########################################################
##               CustusX source
###########################################################

#------------------------------------------------------------------------------
# External libraries
#-----------------------------------------------------------------------------

cx_initialize_Boost()
cx_initialize_QT()
cx_initialize_Eigen()
cx_initialize_VTK()
cx_initialize_ITK()

#if(CX_WINDOWS)
#    cx_initialize_IGSTK()
#endif()

cx_initialize_OpenIGTLink()
cx_initialize_CTK()

cx_initialize_TSF()
cx_initialize_level_set_segmentation()
cx_initialize_OpenCLUtilityLibrary()

find_package(OpenGL)
cx_opengl_version()
find_package(X11)

#------------------------------------------------------------------------------
# Add subdirectories
#------------------------------------------------------------------------------
add_subdirectory(ThirdParty)

add_subdirectory(resource)
add_subdirectory(logic)
add_subdirectory(modules)
add_subdirectory(gui)

add_subdirectory(plugins)

# Testing for CustusX
add_subdirectory(testing)

# Applications: add last (create bundle last).
add_subdirectory(apps)

cxCreateConfigurationDescription()
message(STATUS ${CX_CONFIGURATION_DESCRIPTION})
configure_file(
  "${CustusX_SOURCE_DIR}/source/resource/core/settings/cxConfigDescription.txt.in"
  "${CustusX_BINARY_DIR}/documentation/cxConfigDescription.txt"
  )

configure_file(
  "${CustusX_SOURCE_DIR}/License.txt"
  "${CustusX_BINARY_DIR}/documentation/License.txt"
  )




