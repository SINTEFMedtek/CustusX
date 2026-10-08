NDI Installation {#install_ndi_driver}
===================

CustusX requires that you first install the NDI ToolBox 
(available from [NDI Support](https://support.ndigital.com/downloads.php?filetypebrowse=software)) 
and follow the install instructions. This will install the Track application, which can be used with 
the NDI systems independently of CustusX. *Note: A user account is required to download.*

Windows
-----------------------------------------------------------
Windows install should work out of the box, but you might need to manually install the drivers.
Follow the instructions in the ReadMe file which is in the installation folder of the toolbox,
e.g.:

	C:\Program files (x86)\Northern Digital inc\toolbox\readme.htm


Mac
-----------------------------------------------------------
For OSX 10.9 and higher you need to install the appropriate FTDI driver to get the USB connection working.
Restart the machine after installing: <http://www.ftdichip.com/Drivers/VCP.htm>


Linux (Ubuntu)
-----------------------------------------------------------
No driver installation is needed, but the current user needs access rights to the USB connection.
The CustusX, CustusS, Fraxinus and FraxinusExcelsior install scripts set this up automatically.
To set it up manually, run:

	sudo usermod -a --groups uucp,dialout `whoami`

Log out and back in to make these changes take effect.


Settings
-----------------------------------------------------------
The NDI tracking plugin works without any settings. If needed, these can be added under [General] in
settings.ini in the profile's settings folder (e.g. CustusX_settings/profiles/Laboratory/settings),
while CustusX is closed:

* ndiTrackingHostname: the serial port (e.g. /dev/ttyUSB0 or COM3) or the network address of the
  device. Empty (default): search the serial ports.
* ndiTrackingBaudRate: serial baud rate, default 921600. Slower rates are tried if the device or the
  serial adapter can't use it.
* ndiTrackingIlluminatorRate_Hz: Polaris only, 20, 30 or 60 Hz, default 60. 0 uses the device's
  default (20 Hz). A Polaris that can't use the rate, e.g. a Vicra, tracks at its default rate,
  with a warning in the log.
* ndiTrackingMaxErrorRMS: warn about positions with a larger RMS error (mm). 0 (default): the
  default for the device type.
* ndiTrackingDebugLogging: true to log the commands sent to the device and its replies, in
  NdiCommunication.txt in the logging folder. For Polaris, the number of markers of each tool is
  also written to the log ("NDI Tracker debug" lines) whenever it changes.

Validation
-----------------------------------------------------------
Load your tool ROM-files into Track and verify that they work correctly. There should be no warnings in the Track application: A badly configured tracking system might cause CustusX to fail silently by not receiving tracking data.


