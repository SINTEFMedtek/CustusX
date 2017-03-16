Metrics {#org_custusx_resource_core_metrics}
===================

\addindex metric_widget
Metrics Widget {#utility_widgets_metrics}
===========================================================

3D measurement tools.

Define points, distances and angles.

Several metrics use the same preferences

* Space: Select coordinate system.
* Sample: Sample current tool position.
* Color: Display color.


\addindex point_metric
Point Metrics {#utility_metrics_point}
-----------------------------------------------------------
Point in selected coordinate system. Used by several other metrics.

\addindex frame_metric
Frame Metric
-----------------------------------------------------------
Position and orientation. Stored as 4x4 matrix.

\addindex tool_metric
Tool Metric
-----------------------------------------------------------
Store tool position and orientation. Tool name and offset is also stored.

\addindex distance_metric
Distance Metric
-----------------------------------------------------------
Distance between 2 point metrics. Select the point metrics to use.

\addindex angle_metric
Angle Metric
-----------------------------------------------------------
Angle between 2 lines. Defined as 4 points (defines two vectors).

\addindex plane_metric
Plane Metric
-----------------------------------------------------------
Plane. Defined as 2 points (defines a vector normal to the plane).

\addindex sphere_metric
Sphere Metric
-----------------------------------------------------------
Spere. Defined as 1 point and radius.

\addindex torus_metric
Torus Metric
-----------------------------------------------------------
Torus/donut. Defined with 2 points. 1st point in center of torus. Torus is facing 2nd point.

\addindex custom_metric
Custom Metric
-----------------------------------------------------------
Custom metric. Use with meshes and images and set parameters for how they are displayed. Experimental.

\addindex region_of_interest_metric
Region Of Interest Metric
-----------------------------------------------------------
Used for setting ROI for Camera Style. Experimental.

\addindex export_import_metric
Export and Import metrics to and from a file
-----------------------------------------------------------
It is possible to export all the metrics in the model to an XML file. Such a file can also be imported, but there must be no metrics of the same type in the model at the time of the import.
It is possible to do the import if one edits the file so that the Uids of the metrics in the file are different from the Uids of the metrics in the model.
