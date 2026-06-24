Installation {#installation}
===================

The easiest way to install is to use the installers from the [releases page](https://gitlab.sintef.no/custusx/CustusX/-/releases). Check out the \ref supported_platforms.

Hardware setup:
---------------
- \subpage cx_tracking_setup (using IGSTK)
- \subpage cx_video_setup
- \subpage cx_probe_setup

Tracking using PlusToolkit
---------------

Instead of using the integrated tracking based on IGSTK, the PlusToolkit may be used instead.
See \ref org_custusx_core_openigtlink_plus_install for installation instructions.

CustusX communicates with the PlusServer through OpenIGTLink.
For instructions on how to run PlusServer see https://plustoolkit.github.io.
In addition, CustusX tool config files have to match the config files for PlusServer.
See \ref using_plus_config_files_with_custusx.

