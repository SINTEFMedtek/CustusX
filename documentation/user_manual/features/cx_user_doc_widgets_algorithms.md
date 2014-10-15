Algorithms Widgets {#algorithms_widgets}
===========================================================

[TOC]

\addindex filter_widget
Configurable Filter Widget {#algorithm_widgets_configurable_filter}
===========================================================

Select one type of filter.


TBD

\addindex algorithm_widgets_us_reconstruction
US Reconstruction Widget {#algorithm_widgets_us_reconstruction}
===========================================================

Reconstruct 3D US data from an acquired 2D sequence.

There are several methods. 

TBD.

\addindex calibration_methods_widget
Calibration Widget {#algorithm_widgets_calibration}
===========================================================


\addindex tool_tip_calibrate_widget
Tooltip Calibration Widget {#algorithm_widgets_calibration_tooltip}
-----------------------------------------------------------
Calibrates a tool by sampling it when pointing at a known point on another frame.

> By using the test button you can test your calibration by pointing at a known reference point.


\addindex lap_frame_tool_calibration_widget
Lapframe Calibration Widget {#algorithm_widgets_calibration_lapframe}
-----------------------------------------------------------
Calibrates a tool by sampling it when it is inserted into the slot of the custom-made calibration frame.

The tool to be calibrated will have its calibration set in such a way that the calibrating and calibrated tools
are in the same position.

> By using the test button you can test your calibration by pointing at a known reference point.

\addindex tool_tip_sample_widget
Sample Tooltip Widget {#algorithm_widgets_calibration_sample_tooltip}
-----------------------------------------------------------
Sample the active tool tooltip in any coordinate system and get the results written to file.


\addindex temporal_calibration_widget
Temporal Calibration Widget {#algorithm_widgets_calibration_temporal}
-----------------------------------------------------------
Calibrate the time shift between the tracking system and the video acquisition source.

- Part 1, Acqusition: Move the probe in a sinusoidal pattern up and down in a water tank or similar.
                      The *first* image should be a typical image, as it is used to correlate against all the others.
- Part 2, Calibration: Press calibrate to calculate the temporal shift for the selected acquisition.

The shift is not applied in any way, this must be done manually after calibration. 
Refer to the log folder for the calibration curves.

> **NB:** Previous temporal calibration is applied to probe frames during acqusition.


\addindex tool_manual_calibration_widget
Manual Tool Calibration Widget {#algorithm_widgets_calibration_manual_tool}
-----------------------------------------------------------
Manipulate the tool calibration matrix sMt directly, using the matrix manipulation interface.


\addindex probe_config_widget
Probe Config Calibration Widget {#algorithm_widgets_calibration_probe_config}
-----------------------------------------------------------
View and edit the probe configuration. 

Use the origin to define the position of the probe image in relation 
to the tool t space. Set the cropping box and the probe sector parameters 
(depth and width). All can be written back to the ProbeCalibConfigs.xml file.




\addindex algorithm_widgets_registration_methods
Registration Methods Widget {#algorithm_widgets_registration_methods}
===========================================================

All available registration methods.

TBD