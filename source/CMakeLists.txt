
cmake_minimum_required(VERSION 2.6)

#------------------------------------------------------------------------------
# External libraries
#-----------------------------------------------------------------------------

# Boost
#----------
find_package( Boost REQUIRED )

# Qt
#----------
# find and setup Qt4 for this project
SET (QT_USE_QTXML TRUE)
find_package(Qt4 REQUIRED)
IF(QT_USE_FILE)
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
  SET(QT_LIBRARIES  ${QT_QT_LIBRARY})
ENDIF(QT_USE_FILE)

# VTK
#----------
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# ITK
#----------
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

# IGTL
#----------
FIND_PACKAGE(OpenIGTLink REQUIRED)
INCLUDE(${OpenIGTLink_USE_FILE})


# IGSTK
#----------
FIND_PACKAGE(IGSTK REQUIRED)
INCLUDE(${IGSTK_USE_FILE})

# SSC
#----------
find_package (SSC PATHS "../externals/ssc/CMake" REQUIRED)
#if (SSC_FOUND)
    include (${SSC_USE_FILE})
#	link_directories (${SSC_DIR}/Code)
#endif (SSC_FOUND)
#message(STATUS "verdi av libfolders: " ${SSC_LIBRARY_DIRS} ) 
#----------

#SET(SOURCES cxMain.cpp)
#
#set(SUB_FOLDERS
#    service/video
#    service/patient
#    service/tracking
#    service/visualization
#    logic
#    gui
#    stateMachineManager
#    registrationManager
#    3DUSReconstruction
#    UsReconstruction
#    algorithms
#    )
    
    # removed 20110606 because noone seems to use it
#foreach (ITER ${SUB_FOLDERS})
#    SET(LIB_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/${ITER} ${LIB_DIRECTORIES})    
#endforeach (ITER ${SUB_FOLDERS})
#set (LIB_DIRECTORIES 
#    ${CMAKE_CURRENT_BINARY_DIR}
#    ${LIB_DIRECTORIES}
    #${SSC_DIR}/Code 
#    ${CustusX3_SOURCE_DIR}/modules/cxCommon
#    ${CMAKE_CURRENT_BINARY_DIR}/UsReconstruction/Thunder)
#
# rescope LIB_DIRECTORIES to parent scope
#set(LIB_DIRECTORIES ${LIB_DIRECTORIES} PARENT_SCOPE)

#INCLUDE_DIRECTORIES(
#    .
#    ${SUB_FOLDERS}
#    /usr/local/include
#    ${CustusX3_SOURCE_DIR}/modules/cxCommon
#    gui/infoWidgets # hack
#    gui/dataAdapters
#    )

#include_directories(
#    .
#    gui # move app to other folder
#    )

#------------------------------------------------------------------------------
# Add subdirectories
#-----------------------------------------------------------------------------
add_subdirectory(resource)
add_subdirectory(service)
add_subdirectory(logic)
add_subdirectory(plugins)
add_subdirectory(gui)
add_subdirectory(ThirdParty)

add_subdirectory(algorithms)
#ADD_SUBDIRECTORY(stateMachineManager)
add_subdirectory(registrationManager)
#ADD_SUBDIRECTORY(UsReconstruction)
add_subdirectory(apps)
#IF(APPLE) # doesnt compile on ca's linux box - ??
#    ADD_SUBDIRECTORY(3DUSReconstruction)
#ENDIF(APPLE)


#------------------------------------------------------------------------------
# CPack
#-----------------------------------------------------------------------------
include(CPack)
CPACK_ADD_COMPONENT(CustusX)
CPACK_ADD_COMPONENT(GrabberServer)


#------------------------------------------------------------------------------
# Testing
#-----------------------------------------------------------------------------
add_subdirectory(testing)      
