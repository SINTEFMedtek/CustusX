Liver Surgery Preparation Plugin {#org_custusx_liver}
===================

The liver plugin provides widgets for running TotalSegmentator-based liver
segmentation filters and toggling the resulting structures on and off in
the 2D/3D views.

\addindex liver_segmentation_widget
Liver Segmentation Widget {#org_custusx_liver_segmentation_widget}
-----------------------------------------------------------
Select one or more of the liver segmentation filters (Liver and Pancreas,
Liver Vessels, Liver Lesions, Liver Segments) and run them against one or
two CT/MR volumes (Volume 1 is the active volume; Volume 2 is optional).
Liver Vessels has no MR-capable model, so it is silently skipped (with a
warning) for any MR volume selected.

Large volumes (e.g. whole-body scans) are automatically cropped and, if
still large, resampled to a lower resolution before being sent to
TotalSegmentator, to keep processing time and memory use manageable. This
internal working copy is not shown in volume selectors and is removed once
the run finishes.

Advanced options (toggle with the icon below the filter list):

* **Fast** - uses TotalSegmentator's own low-resolution model. Faster and
  substantially lighter on memory, at the cost of coarser output.
  Recommended for whole-body volumes or memory-constrained machines.
* **TotalSegmentator memory limit** - caps how much memory the
  TotalSegmentator process itself is allowed to use before it is stopped.
  "Automatic" (0) sets this to 60% of the machine's own total physical RAM.

\addindex liver_visibility_widget
Liver Visibility Widget {#org_custusx_liver_visibility_widget}
-----------------------------------------------------------
Toggle visibility of segmented liver structures in the 2D/3D views, scoped
to one source image at a time (the active volume) - useful since running
segmentation against more than one volume can produce several sets of
structures sharing the same organ type. A structure's button is enabled
once that structure has been segmented for the currently selected source
image, and updates automatically as segmentations finish or the active
volume changes.

The "Show/hide in view group" selector controls which of the app's view
groups the show/hide buttons affect (defaulting to view group 0).
