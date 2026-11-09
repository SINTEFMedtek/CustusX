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
#if (APPLE)
#    set(CX_CONFIG_ROOT_RELATIVE_INSTALLED ./config)
#else (APPLE) # Windows and Linux
#    set(CX_CONFIG_ROOT_RELATIVE_INSTALLED ../config)
#endif (APPLE)

#message( STATUS "VTK_RENDERING_BACKEND: " ${VTK_RENDERING_BACKEND} )
if(VTK_RENDERING_BACKEND STREQUAL OpenGL2)
#        message( STATUS "VTK_RENDERING_BACKEND == OpenGL2. setting CX_VTK_OPENGL2" )
        option(CX_VTK_OPENGL2 "VTK build with VTK_RENDERING_BACKEND = OpenGL2" ON)
endif()


# location of configuration files during development (in the source tree)
set (CX_CONFIG_ROOT ${CustusX_SOURCE_DIR}/config)
# hack: the internal project CustusXSetup need a way to inject its own config for use 
# when running from build location. This might be interesting for others as well. 
# TODO: generalize.
#set (CX_OPTIONAL_CONFIG_ROOT ${CustusX_SOURCE_DIR}/../../CustusXSetup/CustusXSetup/config)

set (CX_DATA_ROOT ${CustusX_SOURCE_DIR}/data CACHE PATH "path to CustusX data root")
set (CX_LARGE_DATA_ROOT ${CustusX_SOURCE_DIR}/largedata CACHE PATH "path to CustusX large data root")

# location of documentation files during compilation
set (CX_DOC_ROOT ${CustusX_BINARY_DIR}/doc)
# location of documentation files after install, relative to executable
#if (APPLE)
#    set(CX_DOC_ROOT_RELATIVE_INSTALLED ./doc)
#else (APPLE) # Windows and Linux
#    set(CX_DOC_ROOT_RELATIVE_INSTALLED ../doc)
#endif (APPLE)
  
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cxConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cxConfig.h"
  )

