OpenIGTLink Plugin {#org_custusx_core_openigtlink3}
===================

\addindex network_connections_widget
Network Connections Widget {#org_custusx_core_openigtlink_network_connections_widget}
===========================================================

This widget handles network connections to OpenIGTLink servers or clients and can e.g.
be used to stream video \ref org_custusx_core_video_streamer_openigtlink.<br>
It can only handle 1 connection at a time. Select if you want this application to behave as a server or a client.
Specify the ip and port you want to connect to and selected what kind of OpenIGTLink application you are connecting to.
This is important as different application sends different information in the OpenIGTLink packages.<br>
The plugin supports OpenIGTLink version 3.

Ultrasound streaming and position tracking with Plus Toolkit / OpenIGTLinkIO
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
volume and model data.<br>
This widget is intended primarily for exchanging images and volumes, and has been
tested towards [Slicer3D](https://www.slicer.org).
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
Connect to PlusServer Widget
===========================================================
Widget for connecting to PlusServer easily.
<i>This functionality is currently under development. Some work may be required from the user to get it working.</i>
- Make sure a path to PlusServer is selected.
  For this field to be autoselected requires PlusServer to be compiled/installed on the default location: ```dev/plus-2.6/PlusB-bin/bin/PlusServer```
  (See \ref org_custusx_core_openigtlink_plus_install for instructions).
- Select a Plus config file. This file must correspond to the CustusX tool file.
- Use the `Show PlusServer output` checkbox to show command line output from PlusServer in the console.

Creating Plus config files and matching CustusX tool config files {#using_plus_config_files_with_custusx}
-----------------------------------------------------------
- The *Transform Name* in the transform messages from PlusServer (sent as *deviceName* in transform OpenIGTLink messages)
must correspond to the **openigtlinktransformid** tag in the CustusX tool config file.
- The *deviceName* in Image OpenIGTLink messages must correspond to the **openigtlinkimageid** tag in the CustusX tool config file.
These are in PlusServer created as a combination of **Image Name** and **EmbeddedTransformToFrame**, with an underscore `_` between.


Known issues
-----------------------------------------------------------
### Running PlusServer from CustusX
- The start/stop tracking button is not correctly synchronized when using the PlusServer.
- The CustusX user interface may be unresponsive when PlusServer is starting and stopping.
- The console may print a lot of messages while the PlusServer is starting (<i>"Failed to connect to server..."</i>).
- If show PlusServer output is turned on, a lot of warnings may be printed for the image messages (<i>"Unspecified characters received..."</i>).
- Using Plus tracking may require restart before using other tracking methods in CustusX.
- Pressing stop button before connecting to PlusServer is finished will cause problems (OpenIGTLink client will not be closed).
