cmake_minimum_required(VERSION 2.6)

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES}
        ${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
        ${CustusX3_SOURCE_DIR}/source/resource/videoServer)

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

