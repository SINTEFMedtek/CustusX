cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(SSC REQUIRED)
include (${SSC_USE_FILE})

SET( QT_MOC_HEADER_FILES
    sscMainWindow.h
    )

# library
SET( SOURCE_FILES
    ${QT_MOC_HEADER_FILES}
    main.cpp
    sscMainWindow.cpp
    )

include_directories( 
    .
    ${SNW_INCLUDE_DIRS} 
    ${SSC_BINARY_DIR} 
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic/3DReconstruction    
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms
    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    )	
    
    
    

IF(NOT THUNDER_FRAMEWORK)
  QT4_ADD_RESOURCES(SOURCE_FILES ../../gui/cxResources.qrc)
ENDIF(NOT THUNDER_FRAMEWORK)

IF(CX_WINDOWS)
  QT4_WRAP_CPP( SOURCE_FILES ${QT_MOC_HEADER_FILES} OPTIONS "-D_WINDOWS" )
ELSE(CX_WINDOWS)
  QT4_WRAP_CPP( SOURCE_FILES ${QT_MOC_HEADER_FILES} )
ENDIF(CX_WINDOWS)

# =========================================================
# create the target
IF(APPLE)
  ADD_EXECUTABLE(ReconstructGUI MACOSX_BUNDLE ${SOURCE_FILES} )
ELSE(APPLE)
  ADD_EXECUTABLE(ReconstructGUI ${SOURCE_FILES} )
ENDIF(APPLE)

target_link_libraries( ReconstructGUI cxPluginUSReconstruction )
# =========================================================

#add_executable( sscUsReconstruction MACOSX_BUNDLE ${SOURCE_FILES} )
#target_link_libraries( sscUsReconstruction sscUsReconstructionLib )



