Property Widgets {#property_widgets}
===========================================================

[TOC]

\addindex volume_properties_widget
Volume Properties {#property_widgets_volume}
===========================================================

\addindex clipping_widget 
Volume Clipping {#property_widgets_volume_clipping}
-----------------------------------------------------------
*Functionality for clipping a volume.*

Define clip planes in a volume. The interactive clipper is attached
to the active tool, and clips the active volume according to a slice 
definition. 

The current clip can also be saved along with the volume. 
This can be done several times.


\addindex cropping_widget 
Volume Cropping {#property_widgets_volume_cropping}
-----------------------------------------------------------
 
*Functionality for cropping a volume.*

Lets you crop a volume by defining a bounding box along the volume 
axis. Everything outside the box is not shown.

How to use the bounding box:
 * First click the bounding box to get the control spheres
 * Click and drag a sphere to change the crop area
 
###Group: Auto-crop
Check one or more of the options below, then press the "Auto-crop" button to 
set the bounding box to their combined (intersected) result. This only moves 
the bounding box - press "Create new cropped volume" below to apply it.

* **Crop away outside air** - tightly encloses the body, removing 
  surrounding air/background. The threshold separating body from background 
  is computed adaptively from the volume's own intensity histogram (Otsu's 
  method) rather than a fixed value, so it works for both CT and MR.
* **Crop away head/legs (lung-based, CT only)** - crops to the torso/abdomen 
  region of a whole-body scan, using the lung bases as a landmark. Has no 
  effect if the volume isn't CT or doesn't confidently show the thorax.
* **Crop away head/legs (cross-section)** - a cheaper, modality-agnostic 
  fallback that crops around the widest cross-section of the body (the 
  torso, for any body habitus). Less precise than the lung-based option - 
  e.g. biased toward the chest rather than the abdomen if the arms are down 
  at the sides.

Checking both "outside air" and one of the head/legs options combines a 
lossless x/y crop with a z-axis crop in a single step.

###Button: Create new cropped volume
To make the crop permanent, press the button to create a new volume from the crop.
This will also hide the original volume and show the new volume.

\addindex shading_widget 
Volume Shading {#property_widgets_volume_shading}
-----------------------------------------------------------
 
Set volume shading properties.

\addindex transfer_function_widget
Volume Transfer Function {#property_widgets_volume_transfer_function}
-----------------------------------------------------------

\addindex transfer_function_3d_widget 
### 3D Transfer Function {#property_widgets_volume_transfer_function_3D}
Lets you set a transfer function on a 3D volume. It also lets you set
window width and level, and right click on the bar to bring up
the color selector.

\addindex transfer_function_preset_widget 
### Transfer Function Presets {#property_widgets_volume_transfer_function_presets}
Let you select a predefined transfer function.

\addindex volume_info_widget 
Volume Information {#property_widgets_volume_information}
-----------------------------------------------------------

Displays information about a selected volume.


\addindex slice_properties_widget
Slice Properties {#property_widgets_slice}
===========================================================
Lets you set properties on a 2d image slice.

Slice Information {#property_widgets_slice_information}
-----------------------------------------------------------
Displays information about a selected volume. 
Identical to \ref property_widgets_volume_information.

\addindex color_widget
Slice Color {#property_widgets_slice_color}
-----------------------------------------------------------
Allows setting 2D image properties 
by setting colors and window width and level.
Right click on the bar to bring up the color selector.

\addindex transfer_function_2d_widget 
### 2D Transfer Function {#property_widgets_slice_transfer_function_2D}
Lets you set a transfer function on a 2D volume.

### Transfer Function Slice Presets {#property_widgets_slice_transfer_function_presets}
Let you select a predefined transfer function.
Identical to \ref property_widgets_volume_transfer_function_presets.

\addindex overlay_widget 
Slice 2D Overlay {#property_widgets_slice_overlay}
-----------------------------------------------------------
Allow visualizing several 2D slices  in top of each other 
by making them partly transparent (alpha), 
or by making parts of the fully transparent (LLR).

Not working on Windows.




\addindex tool_properties_widget
Tool Properties {#property_widgets_tool}
===========================================================
Tool offset: A virtual offset along the tool's local z axis.

Tool matrix: The transformation matrix between the tool tip and the selected space.
Change the matrix to move and/or orient the tool. Tool offset is not applied to the matrix.

See \ref coordinate_systems_specific_spaces for more information on coordinate systems.


\addindex tracking_test_widget
\addindex tool_manager_widget
\addindex tool_position_recorder_widget
\addindex tracking_comparison_widget
Tracking Test Widget {#property_widgets_tracking_test}
===========================================================

Tools for testing the tracking, in the Debugging group of the Window menu.

Tracking state
-------------------
Configure, initialize, start and stop tracking, and the reverse, one step at a time.

Record positions
-------------------
Records the tool positions to a CSV file while you press Start/Stop recording, e.g. while moving a tool
in and out of the measurement volume. Each row has a label, the tool, the event (position, visible, hidden
or warning), the tool and receive timestamps (ms), whether the tool is visible, the tool tip position and
rotation in the patient reference space (mm), the position warning, and the tracking system's metadata
(e.g. the NDI RMS error). The widget shows the number of positions, the rate, and the mean position and
spread of each tool.

Compare systems
-------------------
Tracks the same tools with several tracking systems, one after the other: IGSTK, NDI, and OpenIGTLink.
For each system, the tool configuration is copied with that tracking system, tracking is started, the
positions are recorded for the given time, and tracking is stopped. For OpenIGTLink, an OpenIGTLink tracking
server can be started first, e.g. the ndistreamer application from bkinterface, with the given arguments.
The OpenIGTLink tools are matched by the OpenIGTLink transform id in the tool files. Apply the reference to the
OpenIGTLink tools if the server sends the positions in tracker space (e.g. bkinterface), so that the positions
can be compared with the other systems.

The output folder gets one CSV file per system (same format as above), the tool configurations and server logs
that were used, and summary.txt with the rate, mean position and spread of each tool for each system.
Keep the tools still during a comparison, to compare the positions between the systems.


Mesh Properties {#property_widgets_mesh}
===========================================================
\subpage mesh_properties


\addindex eraser_widget
Eraser {#property_widgets_eraser}
===========================================================
Functionality for erasing parts of volumes.
By changing the erase value it is also possible to draw.
Using the eraser on meshes is not possible. 

