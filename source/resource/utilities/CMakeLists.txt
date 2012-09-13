###########################################################
##
##               Utilities Library
## 
##               Resource Layer
##
##  Container for all shared cx resources that doesn't
##  fit in anywhere else.
##
###########################################################


SET(LIBNAME cxUtilities)

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
    cxDisplayTimerWidget.h
    cxPlaybackTime.h
    cxTimelineWidget.h
)

# SSC
#----------
find_package (SSC PATHS ${CustusX3_SOURCE_DIR}/externals/ssc/CMake REQUIRED)
include (${SSC_USE_FILE})

IF(CX_WINDOWS)
  QT4_WRAP_CPP(cxUtilities_MOC_SRCS ${cxUtilities_MOC_HDRS} ${QTUI_HEADERFILES} OPTIONS "-D_WINDOWS")
ELSE(CX_WINDOWS)
  QT4_WRAP_CPP(cxUtilities_MOC_SRCS ${cxUtilities_MOC_HDRS})
ENDIF(CX_WINDOWS)

# _USE_MATH_DEFINES Must be defined before math.h is included
if (CX_WINDOWS)
    add_definitions(-D_USE_MATH_DEFINES)
endif(CX_WINDOWS)

add_library(cxUtilities
    probeXmlConfigParser.h
    probeXmlConfigParser.cpp
    cxCreateProbeDataFromConfiguration.h
    cxCreateProbeDataFromConfiguration.cpp
    cxRenderTimer.h
    cxRenderTimer.cpp
    cxUsReconstructionFileMaker.h
    cxUsReconstructionFileMaker.cpp
    cxUsReconstructionFileReader.h
    cxUsReconstructionFileReader.cpp    
    cxDisplayTimerWidget.h
    cxDisplayTimerWidget.cpp
    cxTransformFile.h
    cxTransformFile.cpp
    cxPlaybackTime.h
    cxPlaybackTime.cpp
    cxTimelineWidget.h
    cxTimelineWidget.cpp
    cxDepthPeeling.h
    cxDepthPeeling.cpp
    ${cxUtilities_MOC_SRCS}
    )
    
include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    )
    
    
target_link_libraries(cxUtilities SSC sscUS3DReconstruction ${QT_LIBRARIES} cxSettings )

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxUtilities)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxUtilities_unity 
        		SSC 
        		sscUS3DReconstruction 
        		${QT_LIBRARIES} 
        		cxSettings
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)

