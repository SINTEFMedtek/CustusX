
# =========================================================
# CX C++ Test App for Qt fun stuff 
# =========================================================

set( MOC_HEADER_FILES
    cxMemoryTester.h
)

set( SOURCE_FILES
    cxMemoryTester.cpp
    cxMemoryTesterMain.cpp
)

qt5_wrap_cpp( MOC_HEADER_FILES ${MOC_HEADER_FILES} )
qt5_add_resources( QT_RESOURCE_FILES resource.qrc )

add_executable(cxMemoryTester MACOSX_BUNDLE
    ${QT_RESOURCE_FILES} 
    ${MOC_HEADER_FILES} 
    ${SOURCE_FILES}
)
target_link_libraries(cxMemoryTester
    PRIVATE
    cxResource
    ${SSC_GCOV_LIBRARY}
    )


