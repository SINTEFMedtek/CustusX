cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Custus unit test folder.
#
# Contains the main catch executable
# =========================================================


if (BUILD_TESTING)
	cx_catch_add_master_exe()
endif (BUILD_TESTING)
