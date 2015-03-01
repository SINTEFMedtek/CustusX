NDI Installation {#install_ndi_driver}
===================

CustusX supports the NDI Polaris and Aurora tracking systems though IGSTK, <http://www.igstk.com>. CustusX requires that
you first install the NDI Track application (available from <http://www.ndigital.com/>) following the NDI 
install instructions. Load the tool ROM-files into Track and verify that they work correctly. There should
be no warnings in the Track application: This will cause CustusX to fail silently by not receiving tracking
data from the tracking system.

If you need to create tool definitions for custom tools, remember to add the ROM-file to each definition. 

Mac
-----------------------------------------------------------
Should work out of the box.

Linux
-----------------------------------------------------------
Run the following shell lines after installing the Track application:

	# Setup access rights for current user
	USER=`whoami`
	sudo usermod -a --groups uucp,dialout ${USER}

	# Setup connection between CustusX and IGSTK
	sudo mkdir -p /Library/CustusX/igstk.links
	sudo chmod a+rwx /Library/CustusX/igstk.links

Windows
-----------------------------------------------------------
CustusX assumes that NDI connects through COM-port 0 (TBD-verify). If this is not the case, change manually.

