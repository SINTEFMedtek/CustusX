add_subdirectory(tests)
add_subdirectory(MemoryTester)
add_subdirectory(OpenIGTLinkServer)
add_subdirectory(PositionFileReader)
add_subdirectory(LogConsole)

# important: add last, required by fixup_bundle on APPLE - must collect all targets first using cx_install_target()
add_subdirectory(CustusX)

