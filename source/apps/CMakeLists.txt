ADD_SUBDIRECTORY(UsReconstruction)
ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(MemoryTester)
add_subdirectory(OpenIGTLinkServer)
add_subdirectory(grabberServer)

# important: add last, required by fixup_bundle on APPLE - must collect all targets first using cx_install_target()
ADD_SUBDIRECTORY(CustusX)

