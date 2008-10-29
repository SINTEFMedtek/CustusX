cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand C++ ToolManager 
# =========================================================

project( snwToolManager )

# =========================================================
# Define variables for paths and include files.
# =========================================================

# Must be the same as the project name. Replaces CMAKE_PROJECT_NAME,
# which I cannot get to work properly
set( SNW_PROJECT_NAME snwToolManager ) 

set( SNW_SOURCE_ROOT_DIR ${PROJECT_SOURCE_DIR}/../.. )

SET( SNW_QT_MOC_HEADER_FILES
    toolManager.h 
    toolThreadSafeData.h 
    toolPositionTrackingThread.h
    )

set( SNW_INCLUDE_DIRS
    /usr/lib64/dbus-1.0/include/ 
    /usr/include/dbus-1.0/  
    ${SNW_SOURCE_ROOT_DIR}/SonowandSintefCooperation/Utilities 
    ${SNW_SOURCE_ROOT_DIR}/SonowandSintefCooperation/Tool 
    ${SNW_SOURCE_ROOT_DIR}/Utilities
    ${SNW_SOURCE_ROOT_DIR}/PersistentData
    ${SNW_SOURCE_ROOT_DIR}/DataBus 
    ${SNW_SOURCE_ROOT_DIR}/DICOMLib 
    ${SNW_SOURCE_ROOT_DIR}/Common 
    ${SNW_SOURCE_ROOT_DIR}/DataStorage 
    ) 

set(SNW_LIBRARY_DIRS 
    /usr/local/dicom/lib 
    ${SNW_SOURCE_ROOT_DIR}/DICOMLib 
    ${SNW_SOURCE_ROOT_DIR}/DataStorage 
    ${SNW_SOURCE_ROOT_DIR}/DataBus 
    ${SNW_SOURCE_ROOT_DIR}/Common 
    )    

set( SNW_LIBRARIES 
    dbus-1  
    DataBusLib 
    snwUtilities
    snwPersistentData
     )

SET( SNW_SOURCE_FILES 
    toolThreadSafeData.cpp 
    toolManager.cpp
    toolToolImpl.cpp
    toolPositionTrackingThread.cpp
    ) 
	
# =========================================================
# create a library
include( ${SNW_SOURCE_ROOT_DIR}/cmake/snwAddLibrary.cmake )
# =========================================================

include( ${SNW_SOURCE_ROOT_DIR}/cmake/snwClearVariables.cmake )

