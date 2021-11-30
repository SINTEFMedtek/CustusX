Installation {#installation}
===================

The easiest way to install is to use the installers from the [website](http://custusx.org/index.php/downloads). Check out the \ref supported_platforms.

Hardware setup:
---------------
- \subpage cx_tracking_setup (using IGSTK)
- \subpage cx_video_setup
- \subpage cx_probe_setup

Troubleshooting:
---------------
- CustusX use OpenCL. If you get errors about OpenCL, e.g. "Missing OpenCL.dll", try installing the latest driver for your graphics card.


Experimental additions:
---------------
Some parts of CustusX are more experimental or unfinished.
These may not be thoroughly tested, be more cumbersome to use,
and some documentation may be lacking.

### Tracking using PlusToolkit

Instead on using the integrated tracking based on IGTSK, the PlusToolkit may be used instead.
See \ref org_custusx_core_openigtlink_plus_install for Ubuntu 16.04 installation instructions.

- CustusX release 18.04 require *PlusToolkit version 2.6*.
- CustusX nightly build (and next release) require *PlusToolkit version 2.7* or newer.

CustusX communicate with the PlusServer through OpenIGTLink.
For instructions on how to run PlusServer see https://plustoolkit.github.io.
In addition, CustusX tool config files have to math the config files for PlusServer.
See \ref using_plus_config_files_with_custusx.
