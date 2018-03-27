OpenIGTLink Plugin {#org_custusx_core_openigtlink3}
===================

\addindex network_connections_widget
Network Connections Widget {#org_custusx_core_openigtlink_network_connections_widget}
===========================================================

This widget handles network connections to OpenIGTLink servers or clients and can e.g.
be used to stream video \ref org_custusx_core_video_streamer_openigtlink.

It can only handle 1 connection at a time. Select if you want this application to behave as a server or a client.
Specify the ip and port you want to connect to and selected what kind of OpenIGTLink application you are connecting to.
This is important as different application sends different information in the OpenIGTLink packages.

The plugin supports OpenIGTLink 3.

Ultrasound streaming and position tracking with PLUS / OpenIGTLinkIO
===========================================================
\ref org_custusx_core_video_streamer_openigtlink can be used to connect to OpenIGTLink servers.

\addindex Object_OpenIGTLink_3
OpenIGTLink3 Widget
===========================================================
Widget imported from OpenIGTLinkIO to test OpenIGTLink connections


\addindex network_data_transfer_widget
Network Data Transfer Widget {#org_custusx_core_openigtlink_datatransfer_widget}
===========================================================

Connect to a remote system using the OpenIGTLink protocol, and send/receive
volume and model data. 

This widget is intended primarily for exchanging images and volumes, and has been
tested towards Slicer3D (<https://www.slicer.org>).
See <http://www.openigtlink.org> for details on the protocol. 

Connection:
- Role: Choose client/server role in the connection, usually client.
- Address/Port: TCP/IP info.
- Connect to: The variant of the protocol to use. For Slicer3D, choose RAS.

Receive:
- Check to automatically accept incoming data into the Patient.

Send:
- Choose data to send to the remove side.

\addindex plus_connect_widget
Connect to PLUS Widget
===========================================================
Widget for connecting to PLUS easily.

- Make sure a path to PlusServer is selected.
  For this field to be autoselected requires PLUS to be compiled/installed on the default location: ```dev/plus-2.6/PlusB-bin/bin/PlusServer``` (See below for instructions).
- Select a PLUS config file.
- Use the `Show PLUS output` checkbox to show command line output from PlusServer in the console.



### Build PLUS 2.6 for CustusX racks with Ubuntu 16.04
Build PLUS on the developer user - then it will be available for all users on the rack computer.

    cd dev
    mkdir plus-2.6
    cd plus-2.6
    git clone https://github.com/PlusToolkit/PlusBuild.git PlusBuild
    cd PlusBuild
    git checkout Plus-2.6

#### Run CMake
- Call bin folder PlusB-bin
- Configure
- (macOS. May need to set CMAKE_MAKE_PROGRAM to: /opt/local/bin/ninja)
- (macOS. May need to set Qt5_DIR to e.g.: /Users/olevs/Qt/5.9.2/clang_64/lib/cmake/Qt5)
- Check Advanced
- (Set variable PLUSLIB_GIT_REVISION to: Plus-2.6)
- (Set variable PLUSAPP_GIT_REVISION to: Plus-2.6)
- Enable PLUS_USE_BKPROFOCUS_VIDEO
- Enable PLUS_USE_NDI
- Generate


    cx ..
    cd PlusB-bin
    ninja

##### Troubleshooting
###### Compiling PLUS 2.6
It seems like PLUS won't compile with CMake 3.5.1 (That comes with Ubuntu 16.04) any longer, because ndicapi now requires CMake 3.6.0 or higher.<br>
A workaround is to edit the file ```plus-2.6/PlusBuild/SuperBuild/External_ndicapi.cmake```:<br>
Change git sha in line 35 to ```c79aec212c0793c3cb0cfb3acd6435ff2ac4cdc1```.

###### Running PlusServer from CustusX
The CustusX user interface may be unresponsive when PlusServer is starting and stopping.
