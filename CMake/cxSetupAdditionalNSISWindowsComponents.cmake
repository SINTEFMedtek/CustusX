# ---------------------------------------------------------------------------
# Windows specific: install scripts + define components
# ---------------------------------------------------------------------------
# Covers both public Fraxinus (CX_SYSTEM_BASE_NAME == "Fraxinus") and
# FraxinusExcelsior (CX_SYSTEM_BASE_NAME == "FraxinusExcelsior", CX_FRAXINUS_PRIVATE
# ON). Before this was added, FraxinusExcelsior silently fell back to CPack's
# stock NSIS template -- no FraxinusPostInstall.nsh, no optional components at
# all -- since its CX_SYSTEM_BASE_NAME stopped matching "Fraxinus" once it was
# renamed (org.custusx.fraxinus.private#1).
if(CX_WINDOWS AND ("${CX_SYSTEM_BASE_NAME}" STREQUAL "Fraxinus" OR CX_FRAXINUS_PRIVATE))

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

  # Relay the NSIS component config (labels, descriptions, pinned tool versions)
  # set as GLOBAL PROPERTYs by FX/org.custusx.fraxinus/CMakeLists.txt back into
  # normal variables here, right before include(CPack) runs in
  # CX/CMake/cxInstallCustusX.cmake. That directory is a sibling of, not an
  # ancestor of, this scope, so a plain set() there never reaches here -- same
  # reason cx_install_apply_customizable_properties() bridges the package icon
  # etc. via GLOBAL PROPERTY instead of set().
  set(_frax_component_props
    CPACK_COMPONENTS_ALL
    CPACK_NSIS_ENABLE_LOG
    CPACK_COMPONENT_CORE_DISPLAY_NAME
    CPACK_COMPONENT_CORE_DESCRIPTION
    CPACK_COMPONENT_RAIDIONICSSETUP_DISPLAY_NAME
    CPACK_COMPONENT_RAIDIONICSSETUP_DESCRIPTION
    CPACK_COMPONENT_TOTALSEGSETUP_DISPLAY_NAME
    CPACK_COMPONENT_TOTALSEGSETUP_DESCRIPTION
    CPACK_COMPONENT_ELASTIXSETUP_DISPLAY_NAME
    CPACK_COMPONENT_ELASTIXSETUP_DESCRIPTION
    CPACK_FRAX_RAIDIONICS_VERSION
    CPACK_FRAX_TOTALSEG_VERSION
    CPACK_FRAX_ELASTIX_VERSION
    CPACK_FRAX_FAMILY_FOLDER_NAME
  )
  foreach(_frax_prop ${_frax_component_props})
    get_property(_frax_prop_val GLOBAL PROPERTY ${_frax_prop})
    if(NOT "${_frax_prop_val}" STREQUAL "")
      set(${_frax_prop} "${_frax_prop_val}")
    endif()
  endforeach()

endif()