
# =========================================================
# app for reading sonowand recorder position files 
# =========================================================

#------------------------------------------------------------------------------
# Variables
#-----------------------------------------------------------------------------
set(SOURCES main.cpp)

cx_add_class_qt_moc(SOURCES
	cxLogConsole
	)
#------------------------------------------------------------------------------
# Include
#-----------------------------------------------------------------------------

include_directories( ${CX_RESOURCE_INCLUDE_DIRECTORIES} )
include_directories(
	.
	${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
	${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
	${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
	${CustusX_SOURCE_DIR}/source/gui
	${CustusX_BINARY_DIR}/source/gui
	${CustusX_SOURCE_DIR}/source/gui/utilities
	${CustusX_SOURCE_DIR}/source/logic
	${CustusX_BINARY_DIR}/source/logic
	${CustusX_SOURCE_DIR}/source/modules/GUIExtender
	${CustusX_BINARY_DIR}/source/modules/GUIExtender
	${CustusX_SOURCE_DIR}/source/modules/Calibration/gui
	${CustusX_BINARY_DIR}/source/modules/Calibration
	${CustusX_SOURCE_DIR}/source/modules/Algorithm/gui
	${CustusX_BINARY_DIR}/source/modules/Algorithm
	${CustusX_SOURCE_DIR}/source/modules/Registration/gui
	${CustusX_BINARY_DIR}/source/modules/Registration

	# To be removed when all old modules have been converted to plugins
	${CustusX_SOURCE_DIR}/source/plugins/org.custusx.usreconstruction
	${CustusX_BINARY_DIR}/source/plugins/org.custusx.usreconstruction
	${CustusX_SOURCE_DIR}/source/plugins/org.custusx.acquisition
	${CustusX_BINARY_DIR}/source/plugins/org.custusx.acquisition
	)

SET(LINK_LIBRARIES
	cxGui
#	cxPluginCalibration
#	cxPluginAlgorithm
	org_custusx_usreconstruction
	org_custusx_acquisition
	cxResource
		)

#cx_remove_duplicate_include_directories()

add_executable(LogConsole ${SOURCES})
target_link_libraries(LogConsole ${LINK_LIBRARIES} )

cx_install_target(LogConsole)
