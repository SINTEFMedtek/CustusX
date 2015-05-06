PNN Reconstuction Plugin {#org_custusx_usreconstruction_pnn}
===================

Overview {#org_custusx_usreconstruction_pnn_overview}
========================

The acquisition plugin enables acquisition of various data streams. 

\addindex pnn
PNN US Reconstruction Algorithm {#org_custusx_usreconstruction_pnn_pnn}
===========================================================

<b>PNN - Pixel Nearest Neighbor reconstruction</b>

Pixel Nearest Neighbor is a simple reconstruction algorithm, and works by iterating over each image plane, and transforming it into the voxel space. In essence, it asks the question “I have this data, where should it go?”. In concrete words, for each pixel on the image plane, the nearest voxel in the voxel grid is found, and the pixel value is put into that voxel. If the voxel already has a value, different approaches are possible: Taking the average, taking the maximum, taking the most recent value, or taking the first value. Usually this is followed by a Hole Filling Step, where the voxels that have no value get a value from the neighboring voxels.

\addtogroup cx_user_doc_group_usreconstruction

* \ref org_custusx_usreconstruction_pnn
