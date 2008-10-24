# =========================================================
# Helper function for creating Sonowand executables.
# Include the file into your project using include().
#
# See file snwPrepareProject.cmake for info about requirements.

# =========================================================
# process the above definitions
include( ${SNW_SOURCE_ROOT_DIR}/CMake/snwPrepareProject.cmake )
# =========================================================
# create the target
add_executable( ${SNW_PROJECT_NAME}	${SNW_GENERATED_SOURCE_FILES} )
target_link_libraries( ${SNW_PROJECT_NAME} ${SNW_GENERATED_LIBRARIES} )
# =========================================================

