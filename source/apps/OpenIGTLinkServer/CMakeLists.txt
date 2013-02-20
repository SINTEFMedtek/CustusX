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
    if (CX_WINDOWS OR CX_LINUX)
        install(TARGETS ${ExecutableName}
            BUNDLE DESTINATION ${CX_INSTALL_ROOT_DIR}
            RUNTIME DESTINATION ${CX_INSTALL_ROOT_DIR}/bin
            )
    endif()

    if(CX_WINDOWS)
            #Installing required run time libs for Windows, msv*.dll
            set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
            include (InstallRequiredSystemLibraries)
            if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
                            install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
                                    DESTINATION bin
                                    PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ
                                    GROUP_EXECUTE WORLD_READ
                                    )
            endif(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)

    endif(CX_WINDOWS)

    #------------------------------------------------------------------------------
    # Bundle utilities
    #------------------------------------------------------------------------------
    # See http://www.itk.org/Wiki/BundleUtilitiesExample for example on how to use bundle utilities

    # Linux
    #----------
    if (CX_LINUX)
            set(APPS "\${CMAKE_INSTALL_PREFIX}/${CX_INSTALL_ROOT_DIR}/bin/OpenIGTLinkServer")
            set(LIBS "")
            set(DIRS
                            ${QT_INCLUDES}
                            ${QT_LIBRARY_DIRS}
                            ${VTK_LIBRARY_DIRS}
                            ${SSC_BINARY_DIR}/Code
                            ${OpenIGTLink_LIBRARY_DIRS}
                            ${OpenCV_LIB_DIR}
                            )

            install(CODE "
                     include(BundleUtilities)
                     fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\") "
                     )

    endif (CX_LINUX)

    # Windows
    #----------
    if (CX_WINDOWS)
            set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${ExecutableName}.exe")
            set(LIBS
                            )
            set(DIRS
                            ${QT_INCLUDES}
                            ${QT_LIBRARY_DIRS}
                            ${QT_BINARY_DIR}
                            ${VTK_LIBRARY_DIRS}
                            ${SSC_BINARY_DIR}/Code
                            ${OpenIGTLink_LIBRARY_DIRS}
                            )
            if(CX_USE_OpenCV)
                    set (DIRS
                                    ${DIRS}
                                    ${OpenCV_LIB_DIR}
                                    )
            endif()
            #message (status "APPS: "${APPS})
            #message (status "LIBS: "${LIBS})
            #message (status "DIRS: "${DIRS})
            #message (status "QT_BINARY_DIR: "${QT_BINARY_DIR})
            install(CODE "
                     include(BundleUtilities)
                     fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\") "
                     )

    endif (CX_WINDOWS)

endif(BUILD_OPEN_IGTLINK_SERVER)

