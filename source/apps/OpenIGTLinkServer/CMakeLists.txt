cmake_minimum_required(VERSION 2.6)

cx_define_option_from_boolean("BUILD_OPEN_IGTLINK_SERVER" "build open igtlink server module" ${CX_LINUX})

if(BUILD_OPEN_IGTLINK_SERVER)

    include_directories(
		.
		${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
		${CustusX3_SOURCE_DIR}/source/resource/videoServer
		${CustusX3_BINARY_DIR}/source/resource/settings
		)

    if(CX_WIN32)
		set(ExecutableName "UltrasonixServer") # windows 32 bit version is made for ultrasonix communication
    else()
		set(ExecutableName "OpenIGTLinkServer")
    endif()

    add_executable( ${ExecutableName}
        main.cpp
        )

    target_link_libraries(${ExecutableName}
        cxGrabber
        )

	cx_install_target(${ExecutableName})

endif(BUILD_OPEN_IGTLINK_SERVER)

