Tracking Setup {#cx_tracking_setup}
===================

[TOC]

IGSTK Setup {#cx_igstk_setup}
===================

CustusX supports the NDI Polaris and Aurora tracking systems though IGSTK, <http://www.igstk.com>. It is assumed that the NDI driver are installed as described here:

\subpage install_ndi_driver .


Platform-specific issues {#cx_tracking_setup_platform_issues}
-----------------------------------------------------------
### Mac and Linux

Setup connection between CustusX and IGSTK:

The folder

	/Library/CustusX/igstk.links

is required to exist and be writable.

You can run the shell commands below after installing the Track application, or create it and set permissions in the Finder.

	sudo mkdir -p /Library/CustusX/igstk.links
	sudo chmod a+rwx /Library/CustusX/igstk.links


### Windows
CustusX assumes that NDI connects through `COM-port` 9. If this is not the case, change the port manually. Go to the device manager in the control panel and
find the NDI USB port under COM Ports. Change the port number in the advanced settings.


Tool File setup {#cx_tool_file}
-----------------------------------------------------------

Each tool needs an associated tool file. Prior to use, the tools must be configured in the `Preferences->Tool Configuration` panel.

A fex tool files are supplied with the application (Look in the `config/tool/Tools/Shared` folder). Use these as a starting point to creating your own.

Important content:

\verbatim
<tool>
  <type>type_of_tool</type>
  <id>unique_uid</id>
  <name>My pointing device</name>
  <clinical_app>Laboratory Laparoscopy</clinical_app>
  <geo_file>visual_representation.stl</geo_file>
  <pic_file>illustrating_image.png</pic_file>

  <sensor>
    <type>Polaris_or_Aurora</type>
    <rom_file>ROMfile.rom</rom_file>
  </sensor>

  <calibration>
    <cal_file>smt.cal</cal_file>
  </calibration>
</tool>
\endverbatim

- **type**: Identifies the type of tool. Must be one of {*reference*, *pointer*, *usprobe*}
- **id**: A unique id used throughout the application.
- **geo_file**: The file `visual_representation.stl` contains a 3D graphical representation of the tool, that will be shown in the 3D scene. The drawing is done in the Tool (t) [space](\ref coordinate_systems).
- **rom_file**: The NDI ROM file for the tool.
- **cal_file**: The file `smt.cal` contains the tool calibration [sMt](\ref coordinate_systems). The `smt.cal` file must contain 16 numbers representing the 4x4 affine matrix [sMt](\ref coordinate_systems). If no file is present, an identity matrix is assumed. 


Troubleshooting {#cx_tracking_setup_troubleshooting}
===================
If you encounter problems:

- Verify that the tracking device works outside of CustusX, using the supplier's native application (e.g. NDI Track)
- Read the error messages in the CustusX Console.
- Check the tool buttons in CustusX, lower right corner.
	- No buttons: Tools are not configured correctly. Check tool files.
	- Gray: Tools are configured correctly, but not connected to hardware.
	- Red: Tool is connected but not visible. Not necessarily a problem. Check that the tool is within the field of view of your hardware.
	- Green: Everything OK and tracking.

Accuracy {#cx_tracking_setup_accuracy}
===================

For a more detailed discussion, refer to (TBD: papers on custusx accuracy).

- Quality of Tool Calibration `sMt`.
- Damage to the tool. If the tool contains a pointing device, verify it is not bent since last calibration
- Accuracy of tracking hardware. Refer to supplier. Some hints:
	- NDI Aurora: Magnetic metal in the FOV, moving to the edge of FOV.
	- NDI Polaris: The spheres should in theory be brand new for every use (NDI recommendation). In practice (outside of the OR) you can reuse spheres, but the accuracy will then drop somewhat, but still be within 1-2mm. Make sure the spheres are not damaged. For example a missing part (or a drop of liquid) on the side might shift the center position by up to half the sphere diameter.
	- NDI Polaris: Position in FOV and orientation of the spheres. 

