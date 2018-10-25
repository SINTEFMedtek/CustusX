Building/installing Plus Toolkit {#org_custusx_core_openigtlink_plus_install}
===================
These build instructions are intended for setting up CustusX together witk PlusServer on
our internal CustusX racks, but can also be used for building PlusServer on
machines that are going to use CustusX together with the Plus Toolkit.


Build Plus 2.6 for CustusX racks with Ubuntu 16.04
===========================================================
Build Plus on the developer user - then it will be available for all users on the rack computer.

    cd dev
    mkdir plus-2.6
    cd plus-2.6
    git clone https://github.com/PlusToolkit/PlusBuild.git PlusBuild
    cd PlusBuild
    git checkout Plus-2.6

Run CMake
-----------------------------------------------------------
- Call bin folder PlusB-bin
- Configure
- (macOS. May need to set CMAKE_MAKE_PROGRAM to: /opt/local/bin/ninja)
- (macOS. May need to set Qt5_DIR to e.g.: /Users/olevs/Qt/5.9.2/clang_64/lib/cmake/Qt5)
- Check Advanced
- Enable PLUS_USE_BKPROFOCUS_VIDEO
- Enable PLUS_USE_NDI
- Generate


    cd ..
    cd PlusB-bin
    make

Let Plus use CustusX rom files
-----------------------------------------------------------
<i>This section is only relevant if you got a CustusX version with Plus tool files.</i>

Plus access the tool rom files in a path relative to the Plus folder structure.
Therefore you will have to copy the rom files from the CustusX installation
to the Plus folder structure:

Copy the following folder from the CustusX installation: ```config/tool```
into the Plus folder structure: ```plus-2.6/PlusB-bin/PlusLibData/ConfigFiles```.

