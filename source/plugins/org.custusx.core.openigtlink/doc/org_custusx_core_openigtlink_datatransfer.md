OpenIGTLink Plugin {#org_custusx_core_openigtlink}
===================

\addindex network_connections_widget
Network Connections Widget {#org_custusx_core_openigtlink_network_connections_widget}
===========================================================

This widget handles network connections to OpenIGTLink servers or clients and can e.g.
be used to stream video \ref org_custusx_core_video_streamer_openigtlink.

It can only handle 1 connection at a time. Select if you want this application to behave as a server or a client.
Specify the ip and port you want to connect to and selected what kind of OpenIGTLink application you are connecting to.
This is important as different application sends different information in the OpenIGTLink packages.


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
