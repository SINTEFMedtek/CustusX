List of Definitions {#definitions}
===================

| Name                    | Description
| ----------------------- | -----------------------------
| Patient                 | The patient is the collection of all data related to a single patient study, such a MR/CT/US volumes, segmentations based on the volumes, and relations between these. All patients in CustusX are anonymous.
| Clinical Application    | A system wide setting that specifies which clinical domain the system is to be used in. Options are Neurology, Laparoscopy etc. This affects the available tools and slice orientations.
| Data set                | Either a volume, surface, or other data displaying 3D information of a patient.
| Metaheader              | Volumetric data format. Consists of two files: .mhd and .raw.
| View                    | A window inside CustusX displaying volume data in either 2D or 3D.
| View Group              | A group of views that display the same data, althoug in different ways.
| Layout                  | One configuration of all visible Views.
| Tool                    | A tracking device, usually from Polaris. Can be a navigation pointer, a surgical device, or an ultrasound probe. Used to navigate within the volumes.
| Manual Tool             | A special Tool that is controlled by the mouse. Otherwise is is treated like a physical tool. Is is the default Pointer when tracking is disabled (when starting the system, for example)
| Slice                   | A 2D slice through a 3D volume. Custus uses several definitions for its slicing, primarily ACS (Axial, Coronal, Sagittal), but also Anyplanes, which are attached to the Tool. Customize using layouts.
| IGT                     | Image Guided Therapy
| Navigation Pointer      | A Tool used for pointing.
