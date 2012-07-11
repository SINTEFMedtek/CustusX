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

IF(CX_WINDOWS)
  QT4_WRAP_CPP(cxSettings_MOC_SRCS ${cxSettings_MOC_HDRS} ${QTUI_HEADERFILES} OPTIONS "-D_WINDOWS")
ELSE(CX_WINDOWS)
  QT4_WRAP_CPP(cxSettings_MOC_SRCS ${cxSettings_MOC_HDRS})
ENDIF(CX_WINDOWS)

# _USE_MATH_DEFINES Must be defined before math.h is included
if (CX_WINDOWS)
    add_definitions(-D_USE_MATH_DEFINES)
endif(CX_WINDOWS)

add_library(cxSettings
    cxDataLocations.cpp
    cxDataLocations.h
    cxSettings.h
    cxSettings.cpp
    ${cxSettings_MOC_SRCS}
    )
    
target_link_libraries(cxSettings SSC ${QT_LIBRARIES} )

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxSettings)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxSettings_unity 
        		SSC 
        		${QT_LIBRARIES}
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)

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