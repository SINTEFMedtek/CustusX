include (CMakeImportBuildSettings)
cmake_import_build_settings (${SSC_BUILD_SETTINGS_FILE})

find_package(Boost REQUIRED)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE})

include_directories( ${SSC_INCLUDE_DIRS}  ${SSC_BINARY_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIRS} )
link_directories (${SSC_LIBRARY_DIRS})
