Preferences
===================


\addindex preferences_general_widget
General {#preferences_general_widget}
===
General preferences

- Patient data folder: All patients folders will be stored here.
- Profile: All settings and configurations will be stored in this profile. Press the "+" button to copy the current one and create a new profile.
- VLC path: Path to the VLC media player. Used for recording videos of the application.
- Tool smoothing: Smooth positions of tracked tools.


\addindex preferences_performance_widget
Performance {#preferences_performance_widget}
===
Parameters affecting the performance of the application.

- Interval between renderings
- Max Render Size: Volumes are downsampled to this size before rendered in 3D (2D still shows slices through original data).
- Smart Render: Render only once per second if there is no changes in the scene.
- 2D Overlay: Enable multiple volumes in 2D (GPU based multi slicer, can only handle up to 4 images).
- Optimized views: Speedup by merging all 2D views into a single vtkRenderWindow
- Depth peeling: Used to show transparent meshes correctly. See also \ref mesh_property_transparent.
- Still Update Rate: Property in vtkRenderWindow. Increasing this value may improve rendering speed at the cost of quality.
- 3D Renderer: Select method for rendering images in 3D


\addindex preferences_automation_widget
Automation {#preferences_automation_widget}
===
Turn on/off automated operations.


\addindex preferences_visualization_widget
Visualization {#preferences_visualization_widget}
===
Preferences for 2D and 3D visualization.


\addindex preferences_video_widget
Video {#preferences_video_widget}
===
Preferences for aqcuired video (with video grabber or digital interface).

- Choose a prefix for the grabbed video.
- Save video in either 24 bit color or 8 bit greyscale (Digital video already in 8 bit will not be converted to 24 bit).
- Compress stored video.


\addindex preferences_tool_config_widget
Tool Configuration {#preferences_tool_config_widget}
===
Setup tool configuration for tracking


\addindex preferences_operating_table_widget
Operating table {#preferences_operating_table_widget}
===
Here you can define the up direction of the operating table. The operating table is a concept which other functionality can use to find up or down directions in the reference space.
It should not be necessary to do any changes here unless you know you are using some of this functionality. See also \ref coordinate_systems.

You can use the predefined Anterior, Posterior and Superior buttons, which will set the table up according to the Dicom coordinate system.
You can also start the tracking and point in the direction towards the earth with the active tool.
Press the *Tool direction is down* button and the software will set the up vector related to the angle the patient is lying in in the reference space.

The table direction can be used together with *Table lock* in the active view group's camera settings, which are available from the *Browser* widget.
The table lock setting aligns the camera's up vector with the table's up vector. This is helpful e.g. when using tool view and you don't want any rotation around the tool axis.
The following figure shows an example of this. The coordinate system indicated is the reference system.
The patient was lying in an angle of about 30 degrees up from a line parallel to the ground. A tool was used to point towards the ground
and the corresponding up vector in the reference space was set as the table's up vector. Since the Table lock is on, the camera up is pointing along the table up and the table appears to be
horizontal down in the scene.

Another use for the table direction is in the *Custom* metric, which by default uses the table up direction to define its own up direction.

\image html cx_operating_table.png "The operating table with the use of Tool direction is down and Table lock"



\addindex preferences_debug_widget
Debug {#preferences_debug_widget}
===
Preferences used for debugging.
