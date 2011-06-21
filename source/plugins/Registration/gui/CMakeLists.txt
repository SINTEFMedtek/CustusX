###########################################################
##               Registration Plugin GUI
##
##               GUI Part
##
###########################################################



SET( MOC_HEADER_FILES
   cxRegistrationPlugin.h
   #cxRegistrationBaseWidget.h
    cxImageRegistrationWidget.h
    cxRegistrationWidget.h
    cxPatientRegistrationWidget.h
    cxRegistrationHistoryWidget.h
    cxManualRegistrationOffsetWidget.h
    cxFastImageRegistrationWidget.h
    #cxFastPatientRegistrationWidget.h
    cxFastOrientationRegistrationWidget.h
    cxImageSegmentationAndCenterlineWidget.h
    cxPlateRegistrationWidget.h
    cxSeansVesselRegistrationWidget.h
    cxRegisterI2IWidget.h
    #cxRegistrationMethodsWidget.h
    cxRegistrationDataAdapters.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
   cxRegistrationPlugin.h
   cxRegistrationPlugin.cpp
   cxRegistrationBaseWidget.h
   cxRegistrationBaseWidget.cpp
    cxImageRegistrationWidget.h
    cxImageRegistrationWidget.cpp
    cxRegistrationWidget.h
    cxRegistrationWidget.cpp
    cxPatientRegistrationWidget.h
    cxPatientRegistrationWidget.cpp
    cxRegistrationHistoryWidget.h
    cxRegistrationHistoryWidget.cpp
    cxManualRegistrationOffsetWidget.h
    cxManualRegistrationOffsetWidget.cpp
    cxFastImageRegistrationWidget.h
    cxFastImageRegistrationWidget.cpp
    cxFastPatientRegistrationWidget.h
    cxFastPatientRegistrationWidget.cpp
    cxFastOrientationRegistrationWidget.h
    cxFastOrientationRegistrationWidget.cpp
    cxImageSegmentationAndCenterlineWidget.h
    cxImageSegmentationAndCenterlineWidget.cpp
    cxPlateRegistrationWidget.h
    cxPlateRegistrationWidget.cpp
    cxSeansVesselRegistrationWidget.h
    cxSeansVesselRegistrationWidget.cpp
    cxRegisterI2IWidget.h
    cxRegisterI2IWidget.cpp    
    cxRegistrationMethodsWidget.h
    cxRegistrationMethodsWidget.cpp  
    cxRegistrationDataAdapters.h
    cxRegistrationDataAdapters.cpp
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
    ${CustusX3_SOURCE_DIR}/source/service/visualization

    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/logic

    ../logic

    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX3_SOURCE_DIR}/source/gui/infoWidgets
    )


# =========================================================
# create the target
add_library( cxPluginRegistrationGUI ${SOURCE_FILES} )
target_link_libraries( cxPluginRegistrationGUI cxPluginAcquisitionGUI cxPluginRegistrationLogic cxPluginAlgorithmGUI)
# =========================================================