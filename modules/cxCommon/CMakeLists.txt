#--------------------------------------------------------------------------------
# Common library for Custus modules.
# 
# Contains shared classes and global definitions of paths etc.
#
#--------------------------------------------------------------------------------

SET(LIBNAME cxCommon)

# Boost
#----------
find_package( Boost REQUIRED )

# Qt
#----------
set(QT_USE_QTXML TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

SET(${LIBNAME}_MOC_HDRS
  UsConfigGui.h
)

IF(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS} ${QTUI_HEADERFILES} OPTIONS "-D_WINDOWS")
ELSE(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS})
ENDIF(WIN32)

add_library(cxCommon
    cxDataLocations.cpp
    cxDataLocations.h
    probeXmlConfigParser.h
    probeXmlConfigParser.cpp
    UsConfigGui.h
    UsConfigGui.cpp
    ${${LIBNAME}_MOC_SRCS}
    )
    
target_link_libraries(cxCommon ${QT_LIBRARIES} )

set (CX_CONFIG_ROOT ${CustusX3_SOURCE_DIR}/config)
set (CX_DATA_ROOT ${CustusX3_SOURCE_DIR}/data CACHE PATH "path to CustusX data root")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cxConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cxConfig.h"
  )