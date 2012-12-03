###########################################################
##
##               Settings Library
## 
##               Resource Layer
##
##  Global defines and settings files     
##
###########################################################

# Select the installed location of configuration files, relative to executable. 
# MUST match the values in CPack install for CustusX!!
if (APPLE) 
    set(CX_CONFIG_ROOT_RELATIVE_INSTALLED ./config)
else (APPLE) # Windows and Linux
    set(CX_CONFIG_ROOT_RELATIVE_INSTALLED ../config)
endif (APPLE)

# location of configuration files during development (in the source tree)
set (CX_CONFIG_ROOT ${CustusX3_SOURCE_DIR}/config)

set (CX_DATA_ROOT ${CustusX3_SOURCE_DIR}/data CACHE PATH "path to CustusX data root")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cxConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cxConfig.h"
  )
