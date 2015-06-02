NDI Installation {#install_ndi_driver}
===================

CustusX requires that you first install the NDI Track application (available from <http://www.ndigital.com/>) following the NDI install instructions. 

Mac and Windows install should work out of the box.

Linux
-----------------------------------------------------------
Run the following shell lines after installing the Track application:

Fedora 21:

	sudo usermod -a --groups uucp,dialout,lock `whoami`
	sudo chown :lock /var/lock # fedora

Ubuntu 14.04:
	sudo usermod -a --groups uucp,dialout `whoami`

This sets up access rights for current user. Logout to make these changes work.

Validation
-----------------------------------------------------------
Load your tool ROM-files into Track and verify that they work correctly. There should be no warnings in the Track application: This will cause CustusX to fail silently by not receiving tracking data from the tracking system.


