###########################################################
##               CustusX source
###########################################################


# Perform full install of package, including config and docs:
include(cxInstallCustusX)

cxCreateConfigurationDescription()
message(STATUS ${CX_CONFIGURATION_DESCRIPTION})
configure_file(
  "${CustusX_SOURCE_DIR}/source/resource/core/settings/cxConfigDescription.txt.in"
  "${CustusX_BINARY_DIR}/doc/cxConfigDescription.txt"
  )

install(FILES
  "${CustusX_BINARY_DIR}/doc/cxConfigDescription.txt"
  "${CustusX_BINARY_DIR}/doc/cxBuiltPlugins.txt"
  DESTINATION ${CX_INSTALL_DOC_DIR}/)

get_property(CX_LICENSE_FILE GLOBAL PROPERTY CX_LICENSE_FILE)

configure_file(
  "${CX_LICENSE_FILE}"
  "${CustusX_BINARY_DIR}/doc/License.txt"
  )




