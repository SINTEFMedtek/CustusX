###########################################################
##               CustusX source
###########################################################

#------------------------------------------------------------------------------
# External libraries
#-----------------------------------------------------------------------------

cx_initialize_Boost()
cx_initialize_QT()
cx_initialize_VTK()
cx_initialize_ITK()
cx_initialize_OpenIGTLink()
cx_initialize_IGSTK()

cx_initialize_ISB_GE()
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
add_subdirectory(service)
add_subdirectory(logic)
add_subdirectory(plugins)
add_subdirectory(gui)

# Testing for CustusX
add_subdirectory(testing)

# Applications: add last (create bundle last).
add_subdirectory(apps)


