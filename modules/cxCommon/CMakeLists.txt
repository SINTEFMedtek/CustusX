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

# SSC
#----------
find_package (SSC PATHS "../externals/ssc/CMake" REQUIRED)
include (${SSC_USE_FILE})

IF(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS} ${QTUI_HEADERFILES} OPTIONS "-D_WINDOWS")
ELSE(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS})
ENDIF(WIN32)

# _USE_MATH_DEFINES Must be defined before math.h is included
if (WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif(WIN32)

add_library(cxCommon
    cxDataLocations.cpp
    cxDataLocations.h
    probeXmlConfigParser.h
    probeXmlConfigParser.cpp
    cxCreateProbeDataFromConfiguration.h
    cxCreateProbeDataFromConfiguration.cpp
    UsConfigGui.h
    UsConfigGui.cpp
    ${${LIBNAME}_MOC_SRCS}
    )
    
target_link_libraries(cxCommon SSC ${QT_LIBRARIES} )

set (CX_CONFIG_ROOT ${CustusX3_SOURCE_DIR}/config)
set (CX_DATA_ROOT ${CustusX3_SOURCE_DIR}/data CACHE PATH "path to CustusX data root")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cxConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cxConfig.h"
  )