Core Widgets {#core_widgets}
===========================================================


Volume manipulation {#core_widgets_volume}
===========================================================

\addindex core_widgets_volume_clipping 
Volume Clipping {#core_widgets_volume_clipping}
-----------------------------------------------------------
*Functionality for clipping a volume.*

Define clip planes in a volume. The interactive clipper is attached
to the active tool, and clips the active volume according to a slice 
definition. 

The current clip can also be saved along with the volume. 
This can be done several times.


\addindex core_widgets_volume_cropping 
Volume Cropping {#core_widgets_volume_cropping}
-----------------------------------------------------------
 
*Functionality for cropping a volume.*

Lets you crop a volume by defining a bounding box along the volume 
axis. Everything outside the box is not shown.

How to use the bounding box:
 * First click the bounding box to get the control spheres
 * Click and drag a sphere to change the crop area
 
> **Tip:** To make the crop permanent, press the button to create a new volume from the crop.

\addindex core_widgets_volume_shading 
Volume Shading {#core_widgets_volume_shading}
-----------------------------------------------------------
 
Set volume shading properties.

Volume Transfer Function {#core_widgets_volume_transfer_function}
-----------------------------------------------------------

\addindex core_widgets_volume_transfer_function_3D 
### 3D Transfer Function {#core_widgets_volume_transfer_function_3D}
Lets you set a transfer function on a 3D volume.

\addindex core_widgets_volume_transfer_function_3D 
### 2D Transfer Function {#core_widgets_volume_transfer_function_2D}
Lets you set a transfer function on a 2D volume.

\addindex core_widgets_volume_information 
Volume Information {#core_widgets_volume_information}
-----------------------------------------------------------

Displays information about a selected volume.

SomePageY {#somepageY}
========================

this is the way to go when referring from qt app
\addindex subtitle_identifier 

Some more text without stars.
Here is a link to [myclass].
Here is a link to #ssc_section_spaces_notation

Next link: [Link text](@ref ssc_section_spaces_notation).

[myclass]: @ref ssc_section_spaces_notation "My class"

