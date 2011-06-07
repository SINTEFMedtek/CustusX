###########################################################
##               US Reconstruction Plugin GUI
##
##               GUI Part
##
###########################################################


SET( MOC_HEADER_FILES
   sscReconstructionWidget.h
   sscReconstructOutputValueParamsInterfaces.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
  sscReconstructionWidget.cpp
  sscReconstructOutputValueParamsInterfaces.cpp
  sscReconstructOutputValueParamsInterfaces.h
  )

# =========================================================

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ../logic
    )


# =========================================================
# create the target
add_library( cxPluginUSReconstructionGUI ${SOURCE_FILES} )
target_link_libraries( cxPluginUSReconstructionGUI cxPluginUSReconstructionLogic )
# =========================================================


