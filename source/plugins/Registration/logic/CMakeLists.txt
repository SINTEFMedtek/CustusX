###########################################################
##               Registration Plugin Logic
##
##               Logic Part
##
###########################################################



SET( MOC_HEADER_FILES
   cxRegistrationManager.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
   cxRegistrationManager.h
   cxRegistrationManager.cpp
   cxLandmarkTranslationRegistration.cpp
  )

# =========================================================


include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms

    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking

    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/logic
    )

# =========================================================
# create the target
add_library( cxPluginRegistrationLogic ${SOURCE_FILES} )
target_link_libraries( cxPluginRegistrationLogic
    cxUtilities 
    cxSettings 
    cxPluginAcquisitionLogic
      )
# =========================================================

#ADD_SUBDIRECTORY( testing )