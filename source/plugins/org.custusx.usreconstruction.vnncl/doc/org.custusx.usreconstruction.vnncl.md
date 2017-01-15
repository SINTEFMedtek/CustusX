VNN Reconstruction Plugin {#org_custusx_usreconstruction_vnncl}
===================

Overview {#org_custusx_usreconstruction_vnncl_overview}
========================

The acquisition plugin enables acquisition of various data streams. 

\addindex vnn_cl
VNNCL US Reconstruction Algorithm {#org_custusx_usreconstruction_vnncl_vnncl}
===========================================================

Different  voxel-based algorithms, meaning that they asks the question “What data should go in this voxel?” for every voxel in the target volume. 


<b>VNN - Voxel Nearest Neighbor</b>
Iterates over the output volume voxels and for each voxel, it finds the image plane that is closest to that voxel, and then the pixel on that plane that is closest to the voxel. There is a maximum radius R involved, where if there is no image plane closer to the voxel than R, the voxel gets no value.

<b>VNN2 - Voxel Nearest Neighbor2</b>
VNN2 is a slightly more complex variant of VNN, where instead of simply taking the closest pixel, all the image planes that are closer than R is obtained, and a distance-weighted average of the closest pixel from each of these planes is computed.


<b>DW - Distance Weighted Reconstruction</b>
Almost the same as VNN2, with the exception that instead of taking the closest pixel on each image plane, one performs bi-linear interpolation on each image plane. Coup´e et. al[Coup´ e et al., 2005] and D. Miller et. al. [Miller et al., 2012]


<b>Anisotropic - Varying Gaussian Distance Weighing</b>
An adaptive algorithm, which tries to intelligently smooth away speckles and noise, yet retains detail in high-frequency regions, while being not being much slower than the above mentioned algorithms. It also has a weight function enabling value collisions to be handled gracefully. 


More details can be found here: http://hdl.handle.net/11250/253677


\addtogroup cx_user_doc_group_usreconstruction

* \ref org_custusx_usreconstruction_vnncl
