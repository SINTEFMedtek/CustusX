Registration Method Plugin: Manual plugin {#org_custusx_registration_method_manual}
===================

Overview {#org_custusx_registration_method_manual_overview}
========================

A collection of manual registration methods.



\addindex manual_patient_registration_widget
Manual Patient Registration {#org_custusx_registration_method_manual_patient}
===========================================================

Direct setting of patient registration

Manipulate the image matrix rMpr via a matrix
or xyz+angles. The writing of the matrix is handled
exactly like an image registration.



\addindex manual_image_2_image_registration_widget
Manual Image to Image Registration {#org_custusx_registration_method_manual_image2image}
===========================================================

Direct setting of image registration.

The matrix is the fMm transform, i.e. from moving to fixed image.



\addindex manual_image_transform_registration_widget
Manual Image Transform {#org_custusx_registration_method_manual_image_transform}
===========================================================

Direct setting of image position

Manipulate the image matrix rMd via a matrix
or xyz+angles. The writing of the matrix is handled
exactly like an image registration.

The rMd matrix is the transform from data/image space to reference space.



\addindex manual_patient_orientation_registration_widget
Manual Patient Orientation {#org_custusx_registration_method_manual_patient_orientation}
===========================================================

Set patient orientation using the navigation tool.

**Prerequisite:** Image registration.

Only orientation of the reference space is changed - data is not moved.
Align the Polaris tool so that the tools tip points towards the patients feet and the
markers face the same way as the patients nose. Click the button. Verify that the 
figure in the upper left corner of the 3D view is correcly aligned.

> Tip: If the patient is orientated with the nose down towards the table, try using *back face*.


