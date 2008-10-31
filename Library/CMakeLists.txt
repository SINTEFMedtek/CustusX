cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ Library 
# =========================================================

project( SSC )

# =========================================================
# Define variables for paths and include files.
# =========================================================

# Must be the same as the project name. Replaces CMAKE_PROJECT_NAME,
# which I cannot get to work properly
set( SNW_PROJECT_NAME SSC ) 

set( SNW_SOURCE_ROOT_DIR ${PROJECT_SOURCE_DIR}/.. )

SET( SNW_QT_MOC_HEADER_FILES
    sscView.h
    Tool/sscToolManager.h
    Tool/sscTool.h 
    Tool/sscDummyTool.h
    Tool/sscDummyToolManager.h
    Rep/sscRep.h
    Rep/sscToolRep3D.h
    )

SET( SNW_HEADER_FILES
    ${SNW_QT_MOC_HEADER_FILES}
    sscRep.h 
    sscRepImpl.h 
    sscVolumeRep.h
    )

set( SNW_INCLUDE_DIRS
    .
    Data
    Rep
    Tool
    Utilities
    ) 

set(SNW_LIBRARY_DIRS 
    )    

set( SNW_LIBRARIES 
    QVTK    
     )

# library
SET( SNW_SOURCE_FILES
    Utilities/sscBoundingBox3D.cpp
    Utilities/sscVector3D.cpp
    Utilities/sscTransform3D.cpp 
    sscView.cpp
    Rep/sscRepImpl.cpp
    Rep/sscVolumetricRep.cpp
    Rep/sscToolRep3D.cpp
    Data/sscImage.cpp
    Data/sscDataManager.cpp
    Tool/sscDummyToolManager.cpp
    Tool/sscDummyTool.cpp
	) 
	
# =========================================================
# create a library
include(${SNW_SOURCE_ROOT_DIR}/CMake/snwAddLibrary.cmake )
# =========================================================


