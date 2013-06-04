###############################################################################
## Installation script for CustusX
#
# Include into CustusX ap dir.
#
###############################################################################

#message(STATUS "CX_INSTALL_ROOT_DIR " ${CX_INSTALL_ROOT_DIR})

include(cxInstallUtilities)

cx_install_configuration_files()
cx_install_documentation_files()

cx_install_windows_runtime_libs(${CX_INSTALL_ROOT_DIR}/bin)

# Install Linux-specific files
if (CX_LINUX)
	install(FILES
			${CustusX3_SOURCE_DIR}/install/Linux/copy/run_v2u.sh
			${CustusX3_SOURCE_DIR}/install/Linux/copy/v2u
			${CustusX3_SOURCE_DIR}/install/Linux/copy/runCustusX.sh
			${CustusX3_SOURCE_DIR}/install/Linux/copy/runOpenIGTLinkServer.sh
			DESTINATION ${CX_INSTALL_ROOT_DIR}
			PERMISSIONS ${CX_FULL_PERMISSIONS})
endif (CX_LINUX)

# Install Apple-specific files
if(APPLE)
	install(FILES ${CustusX3_SOURCE_DIR}/install/Apple/SupportedGrabbers.txt
			DESTINATION ${CX_INSTALL_ROOT_DIR})

		# Install folder for storage of igstk<->CustusX symlinks.
		# The symlinks must be in an absolute location.
		# Apple only (Win uses COM-ports, Linux installs with OS)
		# Because CPACK_PACKAGING_INSTALL_PREFIX is "/", this will install
		# in /Library/CustusX.
		install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/Apple/igstk.links
				DESTINATION Library/CustusX
				FILE_PERMISSIONS ${CX_FULL_PERMISSIONS}
				DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS}
				)
endif(APPLE)

#--------------------------------------------------------------------------------
# Bundle utilities
#--------------------------------------------------------------------------------

# Find the library path of OpenCL - for use with fixup_bundle()
#find_package( OpenCL QUIET)
#message(STATUS OPENCL_FOUND: ${OPENCL_FOUND})
#message(STATUS OPENCL_INCLUDE_DIRS: ${OPENCL_INCLUDE_DIRS})
#message(STATUS OPENCL_LIBRARIES: ${OPENCL_LIBRARIES})
#get_filename_component(OPENCL_LIBRARY_DIRS ${OPENCL_LIBRARIES} PATH)
#message(STATUS cx: OPENCL_LIBRARY_DIRS: ${OPENCL_LIBRARY_DIRS})

if(CX_LINUX)
	set( CustusX_Exe_Path "\${CMAKE_INSTALL_PREFIX}/${CX_INSTALL_ROOT_DIR}/bin/CustusX")
endif()

if(CX_WINDOWS)
	set( CustusX_Exe_Path "\${CMAKE_INSTALL_PREFIX}/bin/CustusX.exe")
endif()

if(APPLE)
	set( CustusX_Exe_Path "\${CMAKE_INSTALL_PREFIX}/${CX_INSTALL_ROOT_DIR}/CustusX.app/Contents/MacOS/CustusX")
endif()

SET(plugin_dest_dir ${CX_INSTALL_ROOT_DIR}/bin)
SET(qtconf_dest_dir ${CX_INSTALL_ROOT_DIR}/bin)
IF(APPLE)
  SET(plugin_dest_dir "${CX_INSTALL_ROOT_DIR}/CustusX.app/Contents/MacOS")
  SET(qtconf_dest_dir "${CX_INSTALL_ROOT_DIR}/CustusX.app/Contents/Resources")
ENDIF(APPLE)


#--------------------------------------------------------------------------------
# Install needed Qt plugins by copying directories from the qt installation
# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats"
		DESTINATION ${plugin_dest_dir}/plugins
		DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS})

#--------------------------------------------------------------------------------

###############################################################################
function(cx_install_additional_executables)
	# Install the OpenIGTLinkServer
	# Only on Apple: Linux/Win does this in the OpenIGTLinkServer folder.
	if(APPLE)
		INSTALL(FILES
					"${CustusX3_BINARY_DIR}/source/apps/GrabberServer/GrabberServer"
				DESTINATION ${plugin_dest_dir}
				PERMISSIONS ${CX_FULL_PERMISSIONS})
	endif()

#	INSTALL(FILES "${CustusX3_BINARY_DIR}/source/apps/OpenIGTLinkServer/OpenIGTLinkServer"
#			DESTINATION ${plugin_dest_dir}
#			PERMISSIONS ${CX_FULL_PERMISSIONS})
#	INSTALL(FILES "${SSC_BINARY_DIR}/Examples/PositionFileReader/sscPositionFileReader"
#				  "${CustusX3_BINARY_DIR}/source/apps/OpenIGTLinkServer/OpenIGTLinkServer"
#			DESTINATION ${plugin_dest_dir}
#			PERMISSIONS ${CX_FULL_PERMISSIONS})
endfunction()

cx_install_additional_executables()

function(cx_install_test_executables)
	INSTALL(FILES "${CustusX3_BINARY_DIR}/source/resource/testing/cxTestResource_CppUnit_CTest"
			DESTINATION ${plugin_dest_dir}
			PERMISSIONS ${CX_FULL_PERMISSIONS})
endfunction()

#cx_install_test_executables()

# collect all installations here. They will be used by fixup_bundle to collect dependencies.
set(CX_EXECUTABLES
#	${plugin_dest_dir}/OpenIGTLinkServer
#	${plugin_dest_dir}/GrabberServer
#	${plugin_dest_dir}/cxTestResource_CppUnit_CTest
#	${plugin_dest_dir}/sscPositionFileReader
	${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}
	)

#--------------------------------------------------------------------------------
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
# This is a requirement to get qt plugins in a relative path working!
INSTALL(CODE "
	file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
	")


set(APPS 
    ${CustusX_Exe_Path}
    #"\${CMAKE_INSTALL_PREFIX}/${CX_INSTALL_ROOT_DIR}/bin/OpenIGTLinkServer"
    )
set(LIBS )
set(DIRS
		${QT_INCLUDES}
		${QT_LIBRARY_DIRS}
		${QT_BINARY_DIR}
		${SSC_INCLUDE_DIRS}
		${GEStreamer_LIBRARY_DIRS}
		${VTK_LIBRARY_DIRS}
		${OpenCV_LIB_DIR}
		${OPENCL_LIBRARY_DIRS}
		${IGSTK_LIBRARY_DIRS}
		${OpenIGTLink_LIBRARY_DIRS}
		${ITK_DIR}/lib
		${DCMTK_DIR}/lib
		${INCLUDE_DIRECTORIES}
		${Tube-Segmentation-Framework_LIBRARY_DIRS}
		${SSC_BINARY_DIR}/Code
		${SSC_BINARY_DIR}/DICOMLib
		${CustusX3_BINARY_DIR}
		${CustusX3_BINARY_DIR}/source
		${CustusX3_BINARY_DIR}/source/service/patient
		${CustusX3_BINARY_DIR}/source/service/state
		${CustusX3_BINARY_DIR}/source/service/tracking
		${CustusX3_BINARY_DIR}/source/service/video
		${CustusX3_BINARY_DIR}/source/service/visualization
		${CustusX3_BINARY_DIR}/source/resource
		${CustusX3_BINARY_DIR}/source/resource/algorithms
		${CustusX3_BINARY_DIR}/source/resource/settings
		${CustusX3_BINARY_DIR}/source/resource/utilities
		${CustusX3_BINARY_DIR}/source/resource/OpenIGTLinkUtilities
		${CustusX3_BINARY_DIR}/source/plugins/Acquisition
		${CustusX3_BINARY_DIR}/source/plugins/Acquisition/logic
		${CustusX3_BINARY_DIR}/source/plugins/Acquisition/gui
		${CustusX3_BINARY_DIR}/source/plugins/Algorithm
		${CustusX3_BINARY_DIR}/source/plugins/Algorithm/logic
		${CustusX3_BINARY_DIR}/source/plugins/Algorithm/gui
		${CustusX3_BINARY_DIR}/source/plugins/Calibration
		${CustusX3_BINARY_DIR}/source/plugins/Calibration/logic
		${CustusX3_BINARY_DIR}/source/plugins/Calibration/gui
		${CustusX3_BINARY_DIR}/source/plugins/Registration
		${CustusX3_BINARY_DIR}/source/plugins/Registration/logic
		${CustusX3_BINARY_DIR}/source/plugins/Registration/gui
		${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction
		${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction/logic
		${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction/logic/3DReconstruction
		${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction/gui
		${CustusX3_BINARY_DIR}/source/gui
		${CustusX3_BINARY_DIR}/source/logic/
		${CustusX3_BINARY_DIR}/source/apps/grabberServer
		${CustusX3_BINARY_DIR}/source/apps/OpenIGTLinkServer
		${CustusX3_BINARY_DIR}/source/resource/videoServer
		)

foreach(EXE ${CX_EXECUTABLES} )
	set(CX_INSTALL_EXES_CODE
		"${CX_INSTALL_EXES_CODE}
		set(TEMP)
		file\(GLOB_RECURSE TEMP \"\${CMAKE_INSTALL_PREFIX}/${EXE}\"\)
		set(QTPLUGINS \${QTPLUGINS} \${TEMP})"
	)
endforeach()

message(STATUS "CX_INSTALL_EXES_CODE" ${CX_INSTALL_EXES_CODE})

# this code appears in cmake_install.cmake in the CURRENT_BINARY_DIR. Check there when changing.
install(CODE "
		${CX_INSTALL_EXES_CODE}
		include(BundleUtilities)
		fixup_bundle(\"${APPS}\"   \"\${QTPLUGINS}\"   \"${DIRS}\") "
		)

#TEST
#set(APPS "\${CMAKE_INSTALL_PREFIX}/${CX_INSTALL_ROOT_DIR}/bin/OpenIGTLinkServer")
# this code appears in cmake_install.cmake in the CURRENT_BINARY_DIR. Check there when changing.
#install(CODE "
#		include(BundleUtilities)
#		fixup_bundle(\"${APPS}\"   \"\${QTPLUGINS}\"   \"${DIRS}\") "
#		)
#TEST END

function(cx_dontuse_fixup)
install(CODE "
		file(GLOB_RECURSE QTPLUGINS
		  \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		  # add openigtlinkserver
		file(GLOB_RECURSE QTPLUGINS
		  \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/OpenIGTLinkServer\")
		file(GLOB_RECURSE QTPLUGINS
		  \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/GrabberServer\")
		include(BundleUtilities)
		fixup_bundle(\"${APPS}\"   \"\${QTPLUGINS}\"   \"${DIRS}\") "
		)
endfunction()

cxCreateConfigurationDescription()
message(STATUS ${CX_CONFIGURATION_DESCRIPTION})

include(CPack)

