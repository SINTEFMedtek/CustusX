#--------------------------------------------------------------------------------
# Common library for Custus modules.
# 
# Contains shared classes and global definitions of paths etc.
#
#--------------------------------------------------------------------------------

# Boost
#----------
find_package( Boost REQUIRED )

# Qt
#----------
set(QT_USE_QTXML TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

add_library(cxCommon
    cxDataLocations.cpp
    cxDataLocations.h
    probeXmlConfigParser.h
    probeXmlConfigParser.cpp
    )
    
    #message(status "custus dir local: " ${CustusX3_SOURCE_DIR})

set (CX_CONFIG_ROOT ${CustusX3_SOURCE_DIR}/config)
set (CX_DATA_ROOT ${CustusX3_SOURCE_DIR}/data)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cxConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cxConfig.h"
  )