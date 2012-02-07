###########################################################
##
##               Settings Library
## 
##               Resource Layer
##
##  Global defines and settings files     
##
###########################################################

# Boost
#----------
find_package( Boost REQUIRED )

# Qt
#----------
set(QT_USE_QTXML TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

SET(cxSettings_MOC_HDRS
  cxSettings.h )

# SSC
#----------
find_package (SSC PATHS ${CustusX3_SOURCE_DIR}/externals/ssc/CMake REQUIRED)
include (${SSC_USE_FILE})

IF(WIN32)
  QT4_WRAP_CPP(cxSettings_MOC_SRCS ${cxSettings_MOC_HDRS} ${QTUI_HEADERFILES} OPTIONS "-D_WINDOWS")
ELSE(WIN32)
  QT4_WRAP_CPP(cxSettings_MOC_SRCS ${cxSettings_MOC_HDRS})
ENDIF(WIN32)

# _USE_MATH_DEFINES Must be defined before math.h is included
if (WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif(WIN32)

add_library(cxSettings
    cxDataLocations.cpp
    cxDataLocations.h
    cxSettings.h
    cxSettings.cpp
    ${cxSettings_MOC_SRCS}
    )
    
target_link_libraries(cxSettings SSC ${QT_LIBRARIES} )

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