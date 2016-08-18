OpenIGTLink Plugin {#org_custusx_core_openigtlink}
===================


\addindex network_data_transfer_widget
Data Transfer Widget {#org_custusx_core_openigtlink_datatransfer_widget}
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


