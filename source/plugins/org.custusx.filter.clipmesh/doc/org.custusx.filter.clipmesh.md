Clip Mesh by Volume {#org_custusx_filter_clipmesh}
===================

\addindex clip_mesh_filter

Removes parts of a mesh that lie in regions where a volume value is at or below a threshold.

Each mesh vertex is probed against the volume. Triangles whose vertices are in regions above
the threshold are kept; triangles that straddle the boundary are split along the isosurface.

The result is stored as a new mesh. The original mesh is not modified.

Typical use
-----------

To keep only the part of a large mesh that falls within a small region of interest:

1. Create or load a binary mask volume (0 outside the region, 1 inside).
2. Select the mesh to clip and the mask volume.
3. Set **Threshold** to 0 (the default) to keep all parts where the volume is non-zero.
4. Run the filter. A new clipped mesh is created.

Parameters
----------

- **Threshold**: Keep mesh parts where the volume value is *above* this value.
  Use 0 for binary masks (values 0 and 1), or increase for multi-value volumes.
  Range: -1000 to 65535 (covers CT Hounsfield units and 8/16-bit volumes).

\addtogroup cx_user_doc_group_filter

* \ref org_custusx_filter_clipmesh
