Acquisition Plugin {#org_custusx_acquisition}
===================


Overview {#org_custusx_acquisition_overview}
========================

The acquisition plugin enables acquisition of various data streams. 

\subpage org_custusx_resource_core_usacquisitionfileformat

\addindex org_custusx_acquisition_widgets_acquisition
US Acquisition Widget {#org_custusx_acquisition_widgets_acquisition}
===========================================================

*Record and reconstruct US data.*<br>
Record a stream of ultrasound images and tracking data, and save to disk. The saved data
can be used to create a 3D volume reconstruction.<br>
For a description of the file format, see \ref org_custusx_resource_core_usacquisitionfileformat
A correctly configured US probe is required to perform the acquisition. See \ref cx_us_probe_definition for more.<br>
<span style="color:red">Note! This widget must be visible during active recording for image and tracking data to be stored.</span>

\addindex sound_speed_converter_widget
Sound Speed Converter Widget {#org_custusx_acquisition_widgets_sound_speed_converter}
===========================================================

Speed of sound compensation.<br>
Calculates a factor to compensate for the difference in sound of speed which a 
ultrasound machine expects and the medium you are doing ultrasound on.



\addindex tracked_centerline_widget
Tracked Centerline Widget {#org_custusx_acquisition_widgets_tracked_centerline}
===========================================================

Record the tool movement as a centerline.
