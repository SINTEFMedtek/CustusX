Tutorial: DICOM Import {#tutorial_dicom_import}
===========================================================

The standard format for medical images is [DICOM](http://medical.nema.org/). This part
of the tutorial assumes that you have access to a DICOM data set containing at least 
one volume. A volume is, in DICOM terms, a series consisting of several 2D images 
arranged in a 3D grid. Such a volume can be imported.

* Open the DICOM Widget
  * `Main Menu -> Window -> Utility -> DICOM`
* Move and enlarge the widget if necessary.
* Press the `Open` button and select a DICOM folder. All DICOM data inside this folder
  and subfolders will now be imported into the widget, but not into CustusX itself.
* Select a series containing a volume. 
  * Optionally use the `Images` column to check if a particular series contains enough images to count as a volume.
  * Optionally use the preview to inspect the images.
  * Optionally use the`View` button to inspect the raw DICOM tags.
* Press the `Import` button to import the selected series into CustusX as a volume.
* Display the volume in the views to verify that the import succeeded.

![DICOM Widget](org_custusx_dicom_widget.jpg)

