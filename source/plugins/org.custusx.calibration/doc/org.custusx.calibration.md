Calibration Plugin {#org_custusx_calibration}
===================

Overview {#org_custusx_calibration_overview}
========================

The calibration plugin is a collection of calibration widgets and methods.




\addindex calibration_methods_widget
Calibration Widget {#algorithm_widgets_calibration}
===========================================================


\addindex tool_tip_calibrate_widget
Tooltip Calibration Widget {#org_custusx_calibration_widgets_tooltip}
-----------------------------------------------------------
Calibrates a tool by sampling it when pointing at a known point on another frame.
This calibration changes only the translational part of the calibration matrix.

> By using the test button you can test your calibration by pointing at a known reference point.


\addindex lap_frame_tool_calibration_widget
Lapframe Calibration Widget {#org_custusx_calibration_widgets_lapframe}
-----------------------------------------------------------
Calibrates a tool by sampling it when it is inserted into the slot of the custom-made Utrecht-SNF-Calibration-tool_POLARIS frame.
This calibration changes both the rotational and translational part of the calibration matrix.

The tool to be calibrated will have its calibration set in such a way that the calibrating and calibrated tools
are in the same position.

> By using the test button you can test your calibration by pointing at a known reference point.

\addindex tool_tip_sample_widget
Sample Tooltip Widget {#org_custusx_calibration_widgets_sample_tooltip}
-----------------------------------------------------------
Sample the active tool tooltip in any coordinate system and get the results written to file.


\addindex temporal_calibration_widget
Temporal Calibration Widget {#org_custusx_calibration_widgets_temporal}
-----------------------------------------------------------
Calibrate the time shift between the tracking system and the video acquisition source.

- Part 1, Acqusition: Move the probe in a sinusoidal pattern up and down in a water tank or similar.
                      The *first* image should be a typical image, as it is used to correlate against all the others.
- Part 2, Calibration: Press calibrate to calculate the temporal shift for the selected acquisition.

<span style="color:red">Note! This widget must be visible during active acquisition for image and tracking data to be stored.</span><br>
The shift is not applied in any way, this must be done manually after calibration.
Refer to the log folder for the calibration curves.<br>

> **NB:** Previous temporal calibration is applied to probe frames during acqusition.


\addindex tool_manual_calibration_widget
Manual Tool Calibration Widget {#org_custusx_calibration_widgets_manual_tool}
-----------------------------------------------------------
Manipulate the tool calibration matrix sMt directly, using the matrix manipulation interface.


Probe Config Calibration Widget {#org_custusx_calibration_widgets_probe_config}
-----------------------------------------------------------
See \ref utility_widgets_probe_config

