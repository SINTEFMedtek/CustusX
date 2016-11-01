add_subdirectory(tests)

cx_add_optional_app_subdirectory("MemoryTester")
cx_add_optional_app_subdirectory("PositionFileReader")
cx_add_optional_app_subdirectory("OpenIGTLinkServer")
cx_add_optional_app_subdirectory("LogConsole")

if(CX_APPLE)
    cx_add_optional_app_subdirectory("OpenGLTestApplication")
endif()

cx_add_optional_app_subdirectory("CustusX")

