###########################################################
##               CustusX Application
###########################################################

#------------------------------------------------------------------------------
# Include
#-----------------------------------------------------------------------------

set(SOURCES cxMain.cpp)

include_directories(
		.
		${CustusX3_SOURCE_DIR}/source/gui
		${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
		${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/gui
		${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/gui
		${CustusX3_SOURCE_DIR}/source/plugins/Calibration/gui
		${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/gui
		${CustusX3_SOURCE_DIR}/source/plugins/Registration/gui
		)

#------------------------------------------------------------------------------
# Set variables
#-----------------------------------------------------------------------------
if(APPLE) # For Apple set the icns file containing icons
	set(MACOSX_BUNDLE_ICON_FILE "application.icns") # set how it shows up in the Info.plist file 
	set_source_files_properties(${CustusX3_SOURCE_DIR}/source/gui/icons/application.icns 
															PROPERTIES MACOSX_PACKAGE_LOCATION Resources)  # set where in the bundle to put the icns file
	
	set(SOURCES ${SOURCES} ${CustusX3_SOURCE_DIR}/source/gui/icons/application.icns)  # include the icns file in the target
	set(MACOSX_BUNDLE_INFO_STRING "CustusX")
	set(MACOSX_BUNDLE_BUNDLE_NAME "CustusX")
	set(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION_NUMBER_VERBOSE}")
	set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${MACOSX_BUNDLE_BUNDLE_VERSION}")
	set(MACOSX_BUNDLE_LONG_VERSION_STRING "CustusX ${MACOSX_BUNDLE_BUNDLE_VERSION}")
	set(MACOSX_BUNDLE_COPYRIGHT "Copyright SINTEF\nAll rights reserved")
endif(APPLE)

#------------------------------------------------------------------------------
# Executable
#-----------------------------------------------------------------------------
if (APPLE)
	add_executable(CustusX MACOSX_BUNDLE ${SOURCES} )
else (APPLE)
	if(CX_WINDOWS)
		add_executable(CustusX ${SOURCES} CustusX.rc)
	else(CX_WINDOWS)
		add_executable(CustusX ${SOURCES} )
	endif(CX_WINDOWS)
endif (APPLE)

#------------------------------------------------------------------------------
# Linking
#------------------------------------------------------------------------------
target_link_libraries( CustusX 
		cxGui 
		cxPluginCalibrationGUI 
		cxPluginAlgorithmGUI 
		cxPluginRegistrationGUI
		cxPluginUSReconstructionGUI 
		cxPluginAcquisitionGUI
		)
		
#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(CustusX)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(CustusX_unity 
        		cxGui 
        		cxPluginCalibrationGUI 
        		cxPluginAlgorithmGUI 
        		cxPluginRegistrationGUI
        		cxPluginUSReconstructionGUI 
        		cxPluginAcquisitionGUI
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)


#--------------------------------------------------------------------------------
# Installer
#--------------------------------------------------------------------------------
# See 
#     http://www.itk.org/Wiki/BundleUtilitiesExample 
# for example on how to use bundle utilities.
#
# More info on Mac+Qt, plugins, and pitfalls related to this:
#     http://lists.trolltech.com/qt-interest/2008-09/thread00258-0.html
#     http://doc.qt.nokia.com/4.7-snapshot/qt-conf.html
#
# The installer creates the following file structure:
#
# CustusX - bin
#         - config - tool
#                  - shader
#                  - settings 
#
# On Apple, the bin folder is replaced by bundles.
#


# Install files
#----------

# Install CustusX
install(TARGETS CustusX
		BUNDLE DESTINATION ${CX_INSTALL_ROOT_DIR}
		RUNTIME DESTINATION ${CX_INSTALL_ROOT_DIR}/bin
		)

# Install configuration files to config folder
install(DIRECTORY ${CustusX3_SOURCE_DIR}/config/ 
		DESTINATION ${CX_INSTALL_ROOT_DIR}/config
		FILE_PERMISSIONS
				OWNER_READ OWNER_EXECUTE OWNER_WRITE
				GROUP_READ GROUP_EXECUTE GROUP_WRITE
				WORLD_READ WORLD_EXECUTE WORLD_WRITE
		DIRECTORY_PERMISSIONS
				OWNER_READ OWNER_EXECUTE OWNER_WRITE
				GROUP_READ GROUP_EXECUTE GROUP_WRITE
				WORLD_READ WORLD_EXECUTE WORLD_WRITE
		PATTERN ".svn" EXCLUDE
		PATTERN "settings/*" EXCLUDE
		)

# Install openCL shaders into bundle
install(FILES ${CustusX3_SOURCE_DIR}/externals/ssc/Code/Ultrasound/3DReconstruction/Thunder/kernels.ocl 
				DESTINATION ${CX_INSTALL_ROOT_DIR}/config/shaders/
				)


# Install Shared files
install(FILES 
	${CustusX3_SOURCE_DIR}/install/Shared/doc/ChangeLog.rtf
	${CustusX3_SOURCE_DIR}/install/Shared/doc/CustusX_Tutorial.pdf
	${CustusX3_SOURCE_DIR}/install/Shared/doc/CustusX_Specifications.pdf
	DESTINATION ${CX_INSTALL_ROOT_DIR}/doc/
	PERMISSIONS 
				OWNER_READ OWNER_EXECUTE OWNER_WRITE
				GROUP_READ GROUP_EXECUTE GROUP_WRITE
				WORLD_READ WORLD_EXECUTE WORLD_WRITE
	)


# Install Windows-specific files
if(CX_WINDOWS)
	# Installing required run time libs for Windows, msv*.dll
	set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
	include (InstallRequiredSystemLibraries)
	if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
			install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
							DESTINATION ${CX_INSTALL_ROOT_DIR}/bin
							PERMISSIONS 
							OWNER_WRITE OWNER_READ 
							OWNER_EXECUTE GROUP_READ
							GROUP_EXECUTE WORLD_READ
		)
	endif(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)

	install(FILES ${CustusX3_SOURCE_DIR}/install/Windows/Windows_Install_ReadMe.rtf 
			DESTINATION ${CX_INSTALL_ROOT_DIR}/doc
			)
endif(CX_WINDOWS)

# Install Linux-specific files
if (CX_LINUX)
	install(FILES 
			${CustusX3_SOURCE_DIR}/install/Linux/copy/run_v2u.sh
			${CustusX3_SOURCE_DIR}/install/Linux/copy/v2u
			#${CustusX3_SOURCE_DIR}/install/Linux/copy/linux_install_readme.txt
			${CustusX3_SOURCE_DIR}/install/Linux/copy/runCustusX.sh
			${CustusX3_SOURCE_DIR}/install/Linux/copy/runOpenIGTLinkServer.sh 
			${CustusX3_SOURCE_DIR}/install/Linux/copy/Fedora_Linux_Installation_Guide.pdf
			DESTINATION ${CX_INSTALL_ROOT_DIR}
		PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
			)
endif (CX_LINUX)

# Install Apple-specific files
if(APPLE)
	install(FILES ${CustusX3_SOURCE_DIR}/install/Apple/apple_install_readme.rtf 
			DESTINATION ${CX_INSTALL_ROOT_DIR}/doc
			)
	install(FILES ${CustusX3_SOURCE_DIR}/install/Apple/SupportedGrabbers.txt
			DESTINATION ${CX_INSTALL_ROOT_DIR}
			)
	install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/Apple/GrabberServer.app 
			DESTINATION ${CX_INSTALL_ROOT_DIR}
			#COMPONENT CustusX
			USE_SOURCE_PERMISSIONS
			)
			
		# Install folder for storage of igstk<->CustusX symlinks. 
		# The symlinks must be in an absolute location.
		# Apple only (Win uses COM-ports, Linux installs with OS)
		# Because CPACK_PACKAGING_INSTALL_PREFIX is "/", this will install
		# in /Library/CustusX.
		install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/Apple/igstk.links 
				DESTINATION Library/CustusX
				FILE_PERMISSIONS
						OWNER_READ OWNER_EXECUTE OWNER_WRITE
						GROUP_READ GROUP_EXECUTE GROUP_WRITE
						WORLD_READ WORLD_EXECUTE WORLD_WRITE
				DIRECTORY_PERMISSIONS
						OWNER_READ OWNER_EXECUTE OWNER_WRITE
						GROUP_READ GROUP_EXECUTE GROUP_WRITE
						WORLD_READ WORLD_EXECUTE WORLD_WRITE
				PATTERN ".svn" EXCLUDE
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
INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins
				DIRECTORY_PERMISSIONS
						OWNER_READ OWNER_EXECUTE OWNER_WRITE
						GROUP_READ GROUP_EXECUTE GROUP_WRITE
						WORLD_READ WORLD_EXECUTE WORLD_WRITE
						)

#--------------------------------------------------------------------------------
# Install the OpenIGTLinkServer
INSTALL(FILES "${CustusX3_BINARY_DIR}/modules/OpenIGTLinkServer/OpenIGTLinkServer" DESTINATION ${plugin_dest_dir}
				PERMISSIONS
						OWNER_READ OWNER_EXECUTE OWNER_WRITE
						GROUP_READ GROUP_EXECUTE GROUP_WRITE
						WORLD_READ WORLD_EXECUTE WORLD_WRITE
						)


#--------------------------------------------------------------------------------
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
INSTALL(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
    ")   
    				
set(APPS ${CustusX_Exe_Path})
set(LIBS )
set(DIRS 
		${QT_INCLUDES} 
		${QT_LIBRARY_DIRS}
		${QT_BINARY_DIR}
		${SSC_INCLUDE_DIRS} 
		${VTK_LIBRARY_DIRS}
		${OpenCV_LIB_DIR}
		${OPENCL_LIBRARY_DIRS}
		${IGSTK_LIBRARY_DIRS}
		${OpenIGTLink_LIBRARY_DIRS}
		${ITK_DIR}/lib
		${DCMTK_DIR}/lib
		${INCLUDE_DIRECTORIES}
		${SSC_BINARY_DIR}/Code
		${SSC_BINARY_DIR}/Code/Ultrasound/3DReconstruction
		${SSC_BINARY_DIR}/DICOMLib
		${CustusX3_BINARY_DIR}
		${CustusX3_BINARY_DIR}/source
		${CustusX3_BINARY_DIR}/source/service/patient
		${CustusX3_BINARY_DIR}/source/service/state
		${CustusX3_BINARY_DIR}/source/service/tracking
		${CustusX3_BINARY_DIR}/source/service/video
		${CustusX3_BINARY_DIR}/source/service/visualization
		${CustusX3_BINARY_DIR}/source/resource/algorithms
		${CustusX3_BINARY_DIR}/source/resource/settings
		${CustusX3_BINARY_DIR}/source/resource/utilities
		${CustusX3_BINARY_DIR}/source/plugins/Acquisition/logic
		${CustusX3_BINARY_DIR}/source/plugins/Acquisition/gui
		${CustusX3_BINARY_DIR}/source/plugins/Algorithm/logic
		${CustusX3_BINARY_DIR}/source/plugins/Algorithm/gui
		${CustusX3_BINARY_DIR}/source/plugins/Calibration/logic
		${CustusX3_BINARY_DIR}/source/plugins/Calibration/gui
		${CustusX3_BINARY_DIR}/source/plugins/Registration/logic
		${CustusX3_BINARY_DIR}/source/plugins/Registration/gui
		${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction/logic
		${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction/gui
		${CustusX3_BINARY_DIR}/source/gui
		${CustusX3_BINARY_DIR}/source/logic/
		)

# old version without plugins: remove after testing on win/linux.	
#		install(CODE "
#				include(BundleUtilities)
#				fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\") "
#				)
install(CODE "
        file(GLOB_RECURSE QTPLUGINS
          \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
          # add openigtlinkserver
        file(GLOB_RECURSE QTPLUGINS
          \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/OpenIGTLinkServer\")
		include(BundleUtilities)
		fixup_bundle(\"${APPS}\"   \"\${QTPLUGINS}\"   \"${DIRS}\") "
		)

		