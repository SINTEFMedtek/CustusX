cmake_minimum_required(VERSION 2.6)

if(BUILD_OPEN_IGTLINK_SERVER)

    #-------------------------------
    # External libs
    #-------------------------------

    # Boost
    #----------

    find_package( Boost REQUIRED )


    # OpenIGTLink
    #----------
    find_package(OpenIGTLink REQUIRED)
    include(${OpenIGTLink_USE_FILE})

    # SSC
    #----------
    find_package(SSC PATHS "../../../externals/ssc/CMake" REQUIRED)
    include(${SSC_USE_FILE})

    #-------------------------------
    # Include directories
    #-------------------------------
    include_directories(
            ${Boost_INCLUDE_DIRS}
    #        ${CustusX3_SOURCE_DIR}/source/ThirdParty/ultrasonix #to find ultrasonix files
    #        ${CustusX3_SOURCE_DIR}/source/resource/settings
    #        ${CustusX3_BINARY_DIR}/source/resource/settings
    #        ${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
            )

    #-------------------------------
    # Linking
    #-------------------------------

    SET(LINK_LIBRARIES
            SSC
            OpenIGTLink
            )

    #-------------------------------
    # Creating the executable
    #-------------------------------
    SET ( cxOpenIGTLinkUtilities_FILES
            cxIGTLinkImageMessage.h
            cxIGTLinkImageMessage.cpp
        cxIGTLinkUSStatusMessage.h
        cxIGTLinkUSStatusMessage.cpp
        igtl_us_status.h
        igtl_us_status.c
    )

    add_library( cxOpenIGTLinkUtilities
        ${cxOpenIGTLinkUtilities_FILES}
        ${MOC_HEADER_FILES}
        )
    target_link_libraries(cxOpenIGTLinkUtilities
        ${LINK_LIBRARIES}
        )

    #add_subdirectory(testing)

endif(BUILD_OPEN_IGTLINK_SERVER)
