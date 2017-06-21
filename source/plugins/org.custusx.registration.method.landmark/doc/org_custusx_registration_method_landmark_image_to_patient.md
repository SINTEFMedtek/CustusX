Landmark Image to Patient Registration Method {#org_custusx_registration_method_landmark_image_to_patient_top}
===================

\addindex org_custusx_registration_method_landmark_image_to_patient_widget
Landmark Image to Patient Registration {#org_custusx_registration_method_landmark_image_to_patient}
===========================================================

This is a set of methods that use landmarks/fiducials to connect the images with the real world.
Landmark image to patient registration will move the patient into the global coordinate system (r).

\addindex org_custusx_registration_method_landmark_image_to_patient_image_landmarks_widget
Image Landmarks
-----------------------------------------------------------

Sample at least 3 landmarks in the data set.
Click the volume and either add new or resample existing landmarks.

\ref org_custusx_registration_method_landmark_import_point_metrics


\addindex org_custusx_registration_method_landmark_image_to_patient_patient_landmarks_widget
Patient Landmarks
-----------------------------------------------------------

Sample points on the patient that corresponds to 3 or more landmarks already sampled in the data set.

Point on the patient using a tool and click the Sample button.
Press the register button to register fixed image.

\addindex org_custusx_registration_method_landmark_image_to_patient_registration_widget
Registration
-----------------------------------------------------------

Register patient to Fixed Volume

Usually the register button don't need to be used as the registration is automatically performed when at lest 3 landmarks are sampled in both the image and patient.
