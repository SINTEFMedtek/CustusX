Registration Method Plugin: Vessel {#org_custusx_registration_method_vessel}
===================

\addindex org_custusx_registration_method_vessel_widget
Vessel Registration {#org_custusx_registration_method_vessel_widget}
========================

This registration method performs a rigid body image to image registration between two vessel structures.

\addindex org_custusx_registration_method_vessel_prepare_widget
Prepare Vessels {#org_custusx_registration_method_vessel_prepare_widget}
===========================================================

Segmentation and centerline extraction for the i2i registration.

Segment out blood vessels from the selected image, then extract the centerline.
When finished, set the result as moving or fixed data in the registration tab.

> The centerline extraction can take a long time.


\addindex org_custusx_registration_method_vessel_i2i_register_widget
\addindex org_custusx_registration_method_vessel_register_widget
\addindex org_custusx_registration_method_vessel_seans_widget
Register {#org_custusx_registration_method_vessel_register_widget}
===========================================================

Registration of vessel segments to each other.

Select two datasets you want to registere to eachother, adjust the input parameters.
Press the register button to perform vessel based registration between fixed and moving images centerlines.

