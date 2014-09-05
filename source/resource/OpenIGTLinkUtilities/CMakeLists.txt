cx_initialize_OpenIGTLink()

include_directories( ${CX_RESOURCE_INCLUDE_DIRECTORIES} )

SET(LINK_LIBRARIES
        cxResource
        OpenIGTLink
    )

SET ( cxOpenIGTLinkUtilities_FILES
        cxIGTLinkImageMessage.h
        cxIGTLinkImageMessage.cpp
        cxIGTLinkUSStatusMessage.h
        cxIGTLinkUSStatusMessage.cpp
        igtl_us_status.h
        igtl_us_status.c
        cxIGTLinkConversion.h
        cxIGTLinkConversion.cpp
    )

add_library( cxOpenIGTLinkUtilities
    ${cxOpenIGTLinkUtilities_FILES}
    ${MOC_HEADER_FILES}
    )
target_link_libraries(cxOpenIGTLinkUtilities
    ${LINK_LIBRARIES}
    )

add_subdirectory(testing)

