Fast Landmark Image to Patient Registration Method {#org_custusx_registration_method_fast_landmark_image_to_patient_top}
===================

\addindex org_custusx_registration_method_fast_landmark_image_to_patient_widget
Fast Image to Patient Registration {#org_custusx_registration_method_fast_landmark_image_to_patient}
===========================================================

Fast Image Registration perform a registration based on tool orientation and one (or more) landmark(s).

The tool is used for setting an approximate patient orientation, and the landmark(s) are used for the translation part of the registration.

Fast image registration will move the moving image to the fixed image.


\addindex org_custusx_registration_method_fast_landmark_image_to_patient_orientation_widget
Orientation {#org_custusx_registration_method_fast_landmark_image_to_patient_orientation}
-----------------------------------------------------------

<b>Prerequisite:</b> Correctly oriented DICOM axes.
Fast and approximate method for orienting the data to the patient.

*Align the Polaris tool so that the tools tip points towards the patients feet and the
markers face the same way as the patients nose. Click the Define Orientation button.*

**Tip:** If the patient is orientated with the nose down towards the table, try using *back face*


\addindex org_custusx_registration_method_fast_landmark_image_to_patient_image_landmarks_widget
Image Landmark(s) {#org_custusx_registration_method_fast_landmark_image_to_patient_image_landmarks}
-----------------------------------------------------------

Select landmarks in the data set that you want to use for performing fast registration.
The landmarks are used for the translation part of the registration.
Click in the dataset and push the add or resample button.

*Only one landmark is required for fast registration.*


\addindex org_custusx_registration_method_fast_landmark_image_to_patient_patient_landmarks_widget
Patient Landmark(s) {#org_custusx_registration_method_fast_landmark_image_to_patient_patient_landmarks}
-----------------------------------------------------------

Select landmarks on the patient that corresponds to one or more of the points sampled in image registration.
The points are used to determine the translation of the patient registration.

Point on the patient using a tool and click the Sample button.
Press the register button to register fixed image.

