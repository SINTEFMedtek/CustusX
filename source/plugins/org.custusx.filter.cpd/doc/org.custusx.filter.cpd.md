Coherent Point Drift (CPD) Registration {#org_custusx_filter_cpd}
===================

\addindex cpd_filter

Registers two meshes using the Coherent Point Drift (CPD) rigid algorithm by Myronenko and Song (2010):
https://ieeexplore.ieee.org/document/5432191

This filter wraps the Python implementation: https://github.com/siavashk/pycpd

The computed rigid transform (rotation + translation) is applied as a registration to the moving mesh.
All data sharing the same frame tree as the moving mesh — volumes, meshes, etc. — will move together.
No new mesh is created.

Dependencies
------------

The filter requires a Python virtual environment with pycpd and numpy.
The environment is created automatically on first use (via `pip install pycpd`).
The virtual environment is stored in the CustusX settings folder under:
`virtualEnvironments/pycpd/venv/`

Usage
-----

Select the **fixed mesh** (target, stays unchanged) and the **moving mesh** (source, will be registered).
Run the filter. The moving mesh and all associated data are moved to the registered position.

Parameters
----------

- **Max iterations**: Maximum number of EM iterations (default: 100)
- **Tolerance**: EM convergence threshold — smaller values require more iterations (default: 1e-3)
- **Outlier weight (w)**: Fraction of points treated as noise rather than valid correspondences.
  Increase for noisy data or when the meshes only partially overlap (default: 0 = all points valid).
  Range 0–0.9.
- **Scale mode**: Controls whether CPD is allowed to optimise a global scale factor.
  - *Rigid* (default): No scaling. Safe for CT/US volumes which cannot be scaled clinically.
  - *Rigid+scale*: CPD optimises scale internally for better point correspondence, but only
    rotation and translation are applied to the data (scale is removed from the result).
  - *Auto*: Tries *Rigid+scale* first; falls back to *Rigid* if the scale deviation exceeds
    the threshold below.
- **Scale threshold**: Used only in *Auto* mode. If `|scale − 1|` exceeds this value the
  registration falls back to *Rigid* (default: 0.1).

\addtogroup cx_user_doc_group_filter

* \ref org_custusx_filter_cpd
