Utility Widgets {#utility_widgets}
===========================================================

[TOC]

\addindex igt_link_widget
Video Connection Widget {#utility_widgets_video_connection}
===========================================================

Setup a connection to a video source. 
\ref cx_user_doc_group_streamer contains a list of all available video sources.





\addindex playback_widget
Playback Widget {#utility_widgets_playback}
===========================================================

View the previously recorded tool movements and video from the current patient.
(video is only recorded during 3D acquisition).

Hover the mouse above the timeline in order to get information about that time. 

- Green indicates visible tools.
- Orange indicates recorded video.
- Black rectangles denote events, such as registrations and data imports.

> **Tip:** Press the red putton to start playback.

> **Note:** The tools in the current configuration must match those stored in the 
> patient you want to playback. If there is a mismatch, a notification will appear
> in the console. Use that to add the correct tools.




\addindex track_pad_widget
Camera Control Widget {#utility_widgets_camera_control}
===========================================================
Trackpad for touch screen devices.
Helps the user control the camera on a touch screen.




\addindex console_widget
Console Widget {#utility_widgets_console}
===========================================================
CustusX console, display device for system administration messages.

*Click the upper left arrow for addition options.*



\addindex navigation_widget
Navigation Widget {#utility_widgets_navigation}
===========================================================
Camera navigation.

Lets you select if the camera should follow a tool.
Click the options to select camera following style.



\addindex probe_config_widget
Probe Config Widget {#utility_widgets_probe_config}
===========================================================
View and edit the probe configuration.

Units are in pixels, except width for sector probes that are in degrees.

Use the origin to define the position of the probe image in relation 
to the tool t space. Set the cropping box and the probe sector parameters 
(depth and width). All can be written back to the ProbeCalibConfigs.xml file.

\addindex sampler_widget
Sampler Widget {#utility_widgets_sampler}
===========================================================
Utility for sampling the current tool point.

Displays the current tool tip position in a selected coordinate system.


\addindex point_sampling_widget
Point Sampling Widget {#utility_widgets_point_sampling}
===========================================================
Utility for sampling points in 3D.
Lets you sample points in 3D and get the distance between sampled points.

Prefer the \ref utility_widgets_metrics instead.
