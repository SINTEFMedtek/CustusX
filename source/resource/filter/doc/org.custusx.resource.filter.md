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

- Optional: Reduce input volume by a factor of 2 in each direction = 1/8 of original volume size
- Marching Cubes contouring
- Optional Windowed Sinc smoothing
- Decimation of triangles
- Optional preserve mesh topology
- Number of iterations in smoothing filter. Higher number = more smoothing
- Band pass width in smoothing filter. Smaller number = more smoothing



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


\addindex color_variation_filter
Color Variation Filter {#org_custusx_resource_filter_color_variation}
===========================================================

*Makes a variation in the mesh color*

This filter creates a mesh where the color to each polygon is
assigned in its .vtk file. The colors are assigned as normal
distributions with its original color (r,g,b: 0-255) as mean.

Options:
- Global variance: The variance of the normal distribution
- Local variance: The maximum color step between two neighbour polygons
- Smoothing: Number of smooting iterations to remove sharp color transitions


\addindex islands_filter
Islands Filter {#org_custusx_resource_filter_islands}
===========================================================

*Divides a binary volume into islands*

vtkImageConnectivityFilter

This filter divides a binary volume into islands. Each separate island is
assigned a label. Largest area is assigned label 1, second largest
number 2, and so on. Input must be a binary volume.
The filter is using vtkImageConnectivityFilter.

Options:
- Min size (voxels): Minimum island size to be labeled and included in output


\addindex meshes_from_labels_filter
Meshes from Labels Filter {#org_custusx_resource_filter_meshes_from_labels}
===========================================================
This filter finds the surfaces of a label volume using discrete marching cubes.
The filter is similar to the Contour Filter, but provides several output meshes.

- Specify start and end label
- Optional: Reduce input volume by a factor of 2 in each direction = 1/8 of original volume size
- Marching Cubes contouring
- Optional Windowed Sinc smoothing
- Decimation of triangles
- Optional preserve mesh topology
- Number of iterations in smoothing filter. Higher number = more smoothing
- Band pass width in smoothing filter. Smaller number = more smoothing
- Specify the same color for all meshes, or optionally generating different colors starting with the selected one

\addtogroup cx_user_doc_group_filter

* \ref org_custusx_resource_filter_dilation
* \ref org_custusx_resource_filter_contour
* \ref org_custusx_resource_filter_resample_image
* \ref org_custusx_resource_filter_smoothing_image
* \ref org_custusx_resource_filter_binary_threshold_image
* \ref org_custusx_resource_filter_color_variation
* \ref org_custusx_resource_filter_islands
* \ref org_custusx_resource_filter_meshes_from_labels

