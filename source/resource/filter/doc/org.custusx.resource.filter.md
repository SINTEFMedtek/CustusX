Standard Filters {#org_custusx_resource_filter}
===================

Overview {#org_custusx_resource_filter_overview}
========================

Standard filters.

\addindex dilation_filter
Dilation Filter {#org_custusx_resource_filter_dilation}
===========================================================
This filter dilates a binary volume with a given radius in mm.

The dilation is performed using a ball structuring element.


\addindex contour_filter
Contour Filter {#org_custusx_resource_filter_contour}
===========================================================
Find the surface of a binary volume using marching cubes.
When you adjust the treshold, you will see a preview of the
filter on the selected input volume.

- Optional factor 2 reduction
- Marching Cubes contouring
- Optional Windowed Sinc smoothing
- Decimation of triangles
- Optional preserve mesh topology



\addindex resample_image_filter
Resample Image Filter {#org_custusx_resource_filter_resample_image}
===========================================================
Resample the volume into the space of the reference volume. Also crop to the same volume.




\addindex smoothing_image_filter
Smoothing Image Filter {#org_custusx_resource_filter_smoothing_image}
===========================================================
*Wrapper for a itk::SmoothingRecursiveGaussianImageFilter.*

Computes the smoothing of an image by convolution with
the Gaussian kernels implemented as IIR filters.
This filter is implemented using the recursive gaussian filters.



\addindex binary_threshold_image_filter
Binary Threshold Image Filter {#org_custusx_resource_filter_binary_threshold_image}
===========================================================

*Segment out areas from the selected image using a threshold.*

This filter produces an output image whose pixels are either one of two values
( OutsideValue or InsideValue ), depending on whether the corresponding input
image pixels lie between the two thresholds ( LowerThreshold and UpperThreshold ).
Values equal to either threshold is considered to be between the thresholds.



\addindex binary_thinning_image_filter_3d_filter
Centerline Filter {#org_custusx_resource_filter_binary_thinning_image}
===========================================================
*Wrapper for a itk::BinaryThinningImageFilter3D.*

This filter computes one-pixel-wide skeleton of a 3D input image.

This class is parametrized over the type of the input image 
and the type of the output image.

The input is assumed to be a binary image. All non-zero valued voxels 
are set to 1 internally to simplify the computation. The filter will 
produce a skeleton of the object.  The output background values are 0, 
and the foreground values are 1.

A 26-neighbourhood configuration is used for the foreground and a
6-neighbourhood configuration for the background. Thinning is performed
symmetrically in order to guarantee that the skeleton lies medial within 
the object.

This filter is a parallel thinning algorithm and is an implementation 
of the algorithm described in:

    T.C. Lee, R.L. Kashyap, and C.N. Chu.
    Building skeleton models via 3-D medial surface/axis thinning algorithms.
    Computer Vision, Graphics, and Image Processing, 56(6):462--478, 1994.


\addtogroup cx_user_doc_group_filter

* \ref org_custusx_resource_filter_dilation
* \ref org_custusx_resource_filter_contour
* \ref org_custusx_resource_filter_resample_image
* \ref org_custusx_resource_filter_smoothing_image
* \ref org_custusx_resource_filter_binary_threshold_image

