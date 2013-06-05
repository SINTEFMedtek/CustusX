cmake_minimum_required(VERSION 2.6)

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

    #-------------------------------
    # Install files
    #-------------------------------
    if (CX_WINDOWS OR CX_LINUX) # mac is handled specially - inside /apps/CustusX
        install(TARGETS ${ExecutableName}
            BUNDLE DESTINATION ${CX_INSTALL_ROOT_DIR}
            RUNTIME DESTINATION ${CX_INSTALL_ROOT_DIR}/bin
            )
    endif()


endif(BUILD_OPEN_IGTLINK_SERVER)

