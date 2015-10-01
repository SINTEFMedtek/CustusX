Registration Method Plugin: Vessel {#org_custusx_registration_method_vessel}
===================

\addindex org_custusx_registration_method_vessel_widget
Vessel Registration {#org_custusx_registration_method_vessel_widget}
========================

This registration method performs a rigid body image to image registration between two vessel structures, using the \ref org_custusx_registration_method_icp_algorithm.

\addindex org_custusx_registration_method_vessel_prepare_widget
Prepare Vessels {#org_custusx_registration_method_vessel_prepare_widget}
===========================================================

Segmentation and centerline extraction for the i2i registration.

Segment out blood vessels from the selected image, then extract the centerline.
When finished, set the result as moving or fixed data in the registration tab.

> The centerline extraction can take a long time.


\addindex org_custusx_registration_method_vessel_register_widget
\addindex org_custusx_registration_method_vessel_seans_widget
Register {#org_custusx_registration_method_vessel_register_widget}
===========================================================

Registration of vessel segments to each other.

Select two data sets you want to register to each other, adjust the input parameters.
Press the register button to perform vessel based registration between fixed and moving point data, using \ref org_custusx_registration_method_icp_algorithm.

ICP Algorithm {#org_custusx_registration_method_icp_algorithm}
========================

The Iterative Closest Point (ICP) Algorithm, also called Vessel Registration, works by iteratively finding the least squares fit between two point data sets, rejecting a fraction of the points during each iteration. This is a *local registration*, meaning that a local minimum will be found, thus the two data sets should be roughly matching.
- Find the intersection of the bounding boxes of the two sets, then remove all points more than a margin outside of this box.
- Iterate until the distances RMS is small enough:
  - Given two data sets, set one as the source and the other as target.
  - Compute minimum distances from each source point to the target cloud
  - Reject a given fraction of the points, remove largest distances first.
  - Perform a least squares fit between the two sets, move one data set accordingly.
  
Parameters:
- **LTS Ratio**: Fraction of points in the source data to keep in each iteration. 
- **Auto LTS**: Ignore LTS ratio, rather find the best value by trying a broad range of values. *Note: Slow.*
- **Margin**: Margin padded to the intersection bounding box before removing points outside.
- **Linear**: Linear/Thinplate registration. *Note: Nonlinear support is experimental.*
- **Display Progress**: Add graphics in the 3D view showing (non-rejected) distance lines between data sets.
- **One Step**: Each registration performs a single iteration only.
