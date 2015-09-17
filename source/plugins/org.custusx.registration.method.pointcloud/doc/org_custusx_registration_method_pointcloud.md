Registration Method Plugin: PointCloud {#org_custusx_registration_method_pointcloud}
===================



\addindex org_custusx_registration_method_pointcloud_frompointer_widget
Surface to Pointer I2P Registration {#org_custusx_registration_method_pointcloud_frompointer_widget}
===========================================================

Use a tracked pointer to indicate the skin surface, then register to a skin surface segmented from image data.

Steps:
- Make sure an initial patient registration is in place (this method is local).
- Segment a skin surface from your image data, then set the surface as *fixed data*.
- Start recording, then move the pointer over the skin area in question. The movement should generate a surface that contains enough variation such that it can be matched to the segmented surface.
- The two surfaces are registered to each other using the \ref org_custusx_registration_method_icp_algorithm, and the patient registration is updated.


\addindex org_custusx_registration_method_pointcloud_widget
Point Cloud I2P Registration {#org_custusx_registration_method_pointcloud_widget}
========================

Use the \ref org_custusx_registration_method_icp_algorithm to perform a patient registration. 
- The *fixed data* is a point cloud in the reference space.
- The *moving data* is interpreted as being in the Patient Reference space, i.e. acquired physical data. 
- The algorithm will perform a registration, then modify the patient registration (see \ref coordinate_systems_specific_spaces) to match the two data sets to each other. 

This is a generalization of the \ref org_custusx_registration_method_pointcloud_frompointer_widget.

Intended usage: Surface-to-surface registration. Extract the surface of an image, then acquire the same surface using physical aquisition such as a laser scanner. Registration will move the surface onto the physical data.
