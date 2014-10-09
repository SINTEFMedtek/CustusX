Tutorial {#tutorial}
===========================================================
\addindex tutorial

The tutorial gives a quick introduction to some key concepts in CustusX, and shoul be enough to get you started with IGT.

It is modeled loosely after a imagined surgery:
The surgeon imports several sets of preoperative data such as MR and CT, adjust the visualization and registers different 
images to each other. Tracking is then started, and the data is registered to the physical patient. The system now has
enough information to be useful during IGT.

- @subpage tutorial_volumes
- @subpage tutorial_image_landmark_registration
- @subpage tutorial_tracking
- @subpage tutorial_patient_landmark_registration

During therapy, CustusX can retrieve intraoperative data that adds to the preoperative volumes. This primarily means
performing an ultrasound recording and creating a 3D reconstructed volume from it.

- @subpage tutorial_video_streaming
- @subpage tutorial_ultrasound_acquisition

It is possible to tune the user interface to suit each and every taste:

- @subpage tutorial_customize_user_interface

If volume data is not available in metaheader format, it is possible to import from DICOM directly:

- @subpage tutorial_dicom_import
