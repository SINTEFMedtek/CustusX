# ---------------------------------------------------------------------------
# Windows specific: install scripts + define components
# ---------------------------------------------------------------------------
if(CX_WINDOWS)

  # Placed Additional install targets in Fraxinus for now
  # See FX/org.custusx.fraxinus/CMakeLists.txt

  # Provide a CPACK_* variable that the NSIS.template.in will substitute
  file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}" _FRAX_BIN_DIR)
  set(CPACK_FRAX_INSTALL_DIR "${_FRAX_BIN_DIR}" CACHE STRING "" FORCE)

  # Tried multiple variants of using CMake/CPack commands, but these didn't work. Like:
  # CPACK_NSIS_EXTRA_INSTALL_COMMANDS
  # CPACK_NSIS_INCLUDE_SCRIPT 
  # CPACK_NSIS_EXECUTABLE_PRE_ARGUMENTS
  # The only solution that worked was to copy and modify NSIS.template.in from CMake

  # --- Ensure our custom NSIS template is used ---
  # Put the directory that contains *NSIS.template.in* at the head of CMAKE_MODULE_PATH
  # so the NSIS generator finds our template first.
  set(CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/cmake/CPackTemplates"
    ${CMAKE_MODULE_PATH})

  # Clean up any previous experiments so they don't interfere:
  unset(CPACK_NSIS_TEMPLATE CACHE)
  unset(CPACK_NSIS_EXTRA_INSTALL_COMMANDS CACHE)
  unset(CPACK_NSIS_EXECUTABLE_PRE_ARGUMENTS CACHE)

endif()