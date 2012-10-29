###########################################################
##               CustusX source
###########################################################

#------------------------------------------------------------------------------
# External libraries
#-----------------------------------------------------------------------------

# Boost
#----------
find_package( Boost REQUIRED )

# Qt
#----------
set(QT_USE_QTXML TRUE)
find_package(Qt4 REQUIRED)
if(QT_USE_FILE)
  include(${QT_USE_FILE})
else(QT_USE_FILE)
  set(QT_LIBRARIES  ${QT_QT_LIBRARY})
endif(QT_USE_FILE)

# VTK
#----------
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# ITK
#----------
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# IGTL
#----------
find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

# IGSTK
#----------
find_package(IGSTK REQUIRED)
include(${IGSTK_USE_FILE})

# SSC
#----------
find_package (SSC PATHS "../externals/ssc/CMake" REQUIRED)
include(${SSC_USE_FILE})

# ISB GE streamer
#----------
option(CX_USE_ISB_GE "use ISB GE scanner interface" OFF)
if (CX_USE_ISB_GE)
    ADD_DEFINITIONS(-DCX_USE_ISB_GE)
    find_package(GEStreamer REQUIRED)
    include(${GEStreamer_USE_FILE})
endif()


#------------------------------------------------------------------------------
# Add subdirectories
#------------------------------------------------------------------------------
add_subdirectory(resource)
add_subdirectory(service)
add_subdirectory(logic)
add_subdirectory(plugins)
add_subdirectory(gui)
add_subdirectory(ThirdParty)
add_subdirectory(apps)

#------------------------------------------------------------------------------
# Testing for CustusX
#-----------------------------------------------------------------------------
add_subdirectory(testing)
