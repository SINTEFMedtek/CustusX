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

cx_initialize_SSC()
cx_initialize_ISB_GE()
cx_initialize_TSF()

#------------------------------------------------------------------------------
# Add subdirectories
#------------------------------------------------------------------------------

add_subdirectory(resource)
add_subdirectory(service)
add_subdirectory(logic)
add_subdirectory(plugins)
add_subdirectory(gui)
add_subdirectory(ThirdParty)

# Testing for CustusX
add_subdirectory(testing)

# Applications: add last (create bundle last).
add_subdirectory(apps)


