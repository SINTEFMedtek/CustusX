Tracking Shape Plugin {#org_custusx_tracking_shape}
===================

Connects to the Fiber Bragg Grating Sensor (FBGS), and shows the shape in CustusX.

\addindex shape_sensor_widget
Shape Sensor Widget {#shape_sensor_widget}
===========================================================

The FBGS hardware is currently only running on a Windows machine. 
CustusX connects to the FBGS software running on this machine through TCP.

A windows version of CustusX can either be run on the FBGS machine, and connenct to IP address 127.0.0.1 (localhost), or run remotely connecting to the IP address of the FBGS machine.

Set up the FBGS machine for a remote TCP clinet (the machine running CustusX).
Open the firewall for the FBGS Shape Sensing application:
* Open **Windows Dedender Firewall** (Press the Windows button and start typing the name, and a suggestion will appear at the top)
* Select **Allow an app or feature through Windows Defender Firewall**
* Find **Shape Sensing** in the list 
* Press **Change settings**
* Make sure **Private** and/or **Public** are selected depending on the network to be used

If the above point isn't enough - Open the firewall for TCP port 5001:
* Open **Windows Defender Firewall with Advanced Security** (Press the Windows button and start typing the name, and a suggestion will appear at the top)
* Click **Inbound rules** (to the left)
* Click **New rule...** (to the right)
* Select **Port**, then **Next >**
* Select **TCP**
* Select **Specific local ports** and enter the number **5001**
* For the next screens: Choose **Allow the connection**, and apply the rule to some or all cases (**Domain**, **Private**, **Public**)
* In the last step give the rule a name like: **FGBS port**
