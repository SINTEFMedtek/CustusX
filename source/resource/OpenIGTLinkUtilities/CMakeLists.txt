cx_initialize_OpenIGTLink()

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

cx_create_export_header("cxOpenIGTLinkUtilities")
cx_remove_duplicate_include_directories()
add_library( cxOpenIGTLinkUtilities
    ${cxOpenIGTLinkUtilities_FILES}
    ${MOC_HEADER_FILES}
    )
target_include_directories(cxOpenIGTLinkUtilities
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(cxOpenIGTLinkUtilities
    ${LINK_LIBRARIES}
    )

add_subdirectory(testing)

