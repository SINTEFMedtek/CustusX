###########################################################
##               US Reconstruction Plugin GUI
##
##               GUI Part
##
###########################################################


SET( MOC_HEADER_FILES
   sscReconstructionWidget.h
   sscReconstructOutputValueParamsInterfaces.h
   cxUsReconstructionPlugin.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
  sscReconstructionWidget.cpp
  sscReconstructOutputValueParamsInterfaces.cpp
  sscReconstructOutputValueParamsInterfaces.h
  cxUsReconstructionPlugin.h
  cxUsReconstructionPlugin.cpp
  )

# =========================================================

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms
    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    ${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/gui
    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ../logic
    )


# =========================================================
# create the target
add_library( cxPluginUSReconstructionGUI ${SOURCE_FILES} )
target_link_libraries( cxPluginUSReconstructionGUI cxPluginUSReconstructionLogic cxPluginAlgorithmGUI)

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginUSReconstructionGUI)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginUSReconstructionGUI_unity 
        		cxPluginUSReconstructionLogic 
        		cxPluginAlgorithmGUI
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
# =========================================================


