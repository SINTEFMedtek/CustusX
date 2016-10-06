Mesh Properties {#mesh_properties_heading}
===========================================================

[TOC]

\addindex active_mesh_widget
Mesh Properties {#mesh_properties}
===========================================================


\addindex mesh_info_widget
Info {#mesh_property_info}
-----------------------------------------------------------
Displays mesh information.

* Uid: Unique id for the mesh
* Name: Displayed name
* Parent Frame: Parent if connected in a frame tree. The mesh will keep its relative position to the parent when registrations are preformed.

The table contain info about various VTK parameters.

* Import Transform from Parent: Replace data transform with that of the parent data.
* Generate Normals: Generate surface normals and add to model. This usually gives a smoother appearance.


\addindex mesh_properties_widget
Properties {#mesh_property_properties}
-----------------------------------------------------------
Set visualization properties of the mesh.

Backface and Frontface culling can be used for making transparent meshes work. Backface only draws outside walls, while frontface only draws inside walls.

Ambient, Diffuse, Specular and Specular Power can be used to change how the mesh responds to lighting.


\addindex mesh_texture_widget
Texture {#mesh_property_texture}
-----------------------------------------------------------
Set texture on the mesh by selecting a previously imported 2D image (File -> Import Data).

Texture shape controls the geometric shape of the texture.
This can only be selected from predefined shapes.

The scale controls how many times to draw the image and the position controls where to draw it.

Experimentation is needed to fit a texture to a mesh and it might not be possible to get a good fit for all meshes.


\addindex select_clippers_for_mesh_widget
Clippers {#mesh_property_clip}
-----------------------------------------------------------
Choose which clippers should be applied to the mesh. Use imvert to clip from the other side.

\addindex mesh_glyphs_widget
Glyphs {#mesh_property_glyph}
-----------------------------------------------------------
Glyph visualization.
(TBD.)
