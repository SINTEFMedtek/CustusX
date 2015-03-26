Registration method: Vessel plugin {#org_custusx_registration_method_vessel}
===================

Overview {#org_custusx_registration_method_vessel_overview}
========================

This registration method performs a rigid body image to image registration between two vessel structures.

\addindex prepare_vessels_widget
Prepare Vessels Widget {#org_custusx_registration_method_vessel_widget_prepare}
===========================================================

Segmentation and centerline extraction for the i2i registration.

Segment out blood vessels from the selected image, then extract the centerline.
When finished, set the result as moving or fixed data in the registration tab.

> The centerline extraction can take a long time.


\addindex register_i2i_widget
Register I2I Widget {#org_custusx_registration_method_vessel_widget_register_i2i}
===========================================================

Registration of vessel segments to eachother.
Press the button to perform vessel based registration between image 1 and image 2s centerlines.

\addindex seans_vessel_registration_widget
Vessel Registration Widget  {#org_custusx_registration_method_vessel_widget_seans}
-----------------------------------------------------------

Select two datasets you want to registere to eachother, adjust the input parameters.

Adjust the parameters and click the register button.


