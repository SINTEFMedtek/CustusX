Landmark Import Point Metrics {#org_custusx_registration_method_landmark_import_point_metrics_top}
===================

Import point metrics as landmarks {#org_custusx_registration_method_landmark_import_point_metrics}
===========================================================

The *Import Point Metrics* button in the advanced options can be used to import point metrics in the patient as landmarks.
There are some conditions which controls how this works:
- The point metrics to be imported must have the *Fixed Data* which has been selected set as their parent space.
- For the landmarks to be created with the corresponding coordinates of the metrics,
the name of the metrics has to be a simple number. I.e. 1, 2, 3... In this way, landmark 1 will get the coordinates of the metric with the name 1 and so on.
- If you have metrics with other names (but with the correct parent space) they will be imported as landmarks which must be sampled manually.
- The same if you have given a name which ia a simple number, but the number is higher than the total number of metrics to be imported.
