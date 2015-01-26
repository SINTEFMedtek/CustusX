set(ExecutableName "OpenIGTLinkServer")
if(CX_WIN32)
        set(ExecutableName "UltrasonixServer") # windows 32 bit version is made for ultrasonix communication
endif()

set(BUNDLE_TYPE "")
if(CX_APPLE)
    set(BUNDLE_TYPE "MACOSX_BUNDLE")
endif()

add_executable( ${ExecutableName} ${BUNDLE_TYPE} main.cpp)

target_link_libraries(${ExecutableName}
    PUBLIC
    cxGrabber
    )

cx_install_target(${ExecutableName})

