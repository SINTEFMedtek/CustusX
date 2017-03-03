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


Linux
-----------------------------------------------------------
Run the following shell lines after installing the Track application:

### Fedora

	sudo usermod -a --groups uucp,dialout,lock `whoami`
	sudo chown :lock /var/lock

### Ubuntu

	sudo usermod -a --groups uucp,dialout `whoami`

This sets up access rights for current user. Logout to make these changes work.


Validation
-----------------------------------------------------------
Load your tool ROM-files into Track and verify that they work correctly. There should be no warnings in the Track application: A badly configured tracking system might cause CustusX to fail silently by not receiving tracking data.


