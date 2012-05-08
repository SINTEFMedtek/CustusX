cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ Library 
# =========================================================

project( SSC )


enable_testing()

include_directories( /usr/local/include )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/CMake)

#-----------------------------------------------------------------------------
# Cotire (COmpile TIme REducer)
# Cross-platform cmake module for reducing compile time by generating 
# precomiled headers (pch) and single compilation unit builds
# https://github.com/sakra/cotire
#-----------------------------------------------------------------------------
option(COTIRE_ENABLE_PRECOMPILED_HEADERS "use cotire to autogenerate precompiled headers (pch)" OFF)
option(COTIRE_ADD_UNITY_BUILDS "use cotire to autogenerate single comilation unit builds" OFF)

if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    include(cotire)
    # The properties COTIRE_ENABLE_PRECOMPILED_HEADER and COTIRE_ADD_UNITY_BUILD can also be set on targets. 
    # A target inherits the property value from its enclosing directory.
    set_directory_properties(
                PROPERTIES 
                COTIRE_ENABLE_PRECOMPILED_HEADER ${COTIRE_ENABLE_PRECOMPILED_HEADERS} 
                COTIRE_ADD_UNITY_BUILD ${COTIRE_ADD_UNITY_BUILDS}
                )
endif()
#-----------------------------------------------------------------------------

add_subdirectory(Code)
add_subdirectory(DICOMLib)
add_subdirectory(Examples)
add_subdirectory(Testing)
