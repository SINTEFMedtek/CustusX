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

cx_initialize_OpenIGTLink()
cx_initialize_OpenIGTLinkIO()
cx_initialize_CTK()

#if(CX_LINUX)
#   cx_initialize_opencl()
#endif(CX_LINUX)

#cx_initialize_FAST()

#Turn off all OpenCL use in CustusX
#cx_initialize_OpenCLUtilityLibrary()
set(CX_USE_OPENCL_UTILITY OFF)
set(CX_PLUGIN_org.custusx.usreconstruction.vnncl OFF)

#Turn off obsolete plugins
set(CX_PLUGIN_org.custusx.anglecorrection OFF)

find_package(OpenGL)
cx_opengl_version()
find_package(X11)

#------------------------------------------------------------------------------
# Add subdirectories
#------------------------------------------------------------------------------
add_subdirectory(ThirdParty)

add_subdirectory(resource)
add_subdirectory(logic)
add_subdirectory(gui)

add_subdirectory(plugins)

# Testing for CustusX
add_subdirectory(testing)

# Applications: add last
add_subdirectory(apps)

# perform global installation tasks last, such as install bundle (must be in subdir to ensure correct calling order)
add_subdirectory(installation)


