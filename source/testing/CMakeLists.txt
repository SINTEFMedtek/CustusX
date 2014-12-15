# =========================================================
# Custus unit test folder.
#
# Contains the main catch executable
# =========================================================


if(BUILD_TESTING)

    set(CX_CATCH_SOURCES
        ${CustusX_SOURCE_DIR}/source/ThirdParty/catch/catch.hpp
        cxtestCatchImpl.h
        cxtestCatchImpl.cpp
    )
    add_library(cxCatch ${CX_CATCH_SOURCES})
    include(GenerateExportHeader)
    generate_export_header(cxCatch)
    target_include_directories(cxCatch
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CustusX_SOURCE_DIR}/source/ThirdParty/catch
    )
    target_link_libraries(cxCatch
        PRIVATE
        cxResource
    )
    cx_add_executable_catch(cxCatch)

endif(BUILD_TESTING)
