Coherent Point Drift (CPD) Registration {#org_custusx_filter_cpd}
===================

\addindex cpd_filter

Coherent Point Drift (CPD) Registration {#org_custusx_filter_cpd_title}
---

Registers two meshes using the Coherent Point Drift (CPD) rigid algorithm by Myronenko and Song (2010):
https://ieeexplore.ieee.org/document/5432191

This filter wraps the Python implementation: https://github.com/siavashk/pycpd

The computed rigid transform (rotation + translation) is applied as a registration to the moving mesh.
All data sharing the same frame tree as the moving mesh — volumes, meshes, etc. — will move together.
No new mesh is created.

#### Dependencies

The filter requires a Python virtual environment with pycpd and numpy.
The environment is created automatically on first use (via `pip install pycpd`).
The virtual environment is stored at:

    <CustusX settings folder>/virtualEnvironments/pycpd/venv/

#### Usage

Select the **fixed mesh** (target, stays unchanged) and the **moving mesh** (source, will be registered).
Run the filter. The moving mesh and all associated data are moved to the registered position.

#### Parameters

- **Max iterations**: Maximum number of EM iterations (default: 100)
- **Tolerance**: Convergence tolerance — smaller values give more precise results (default: 1e-5)

\addtogroup cx_user_doc_group_filter

* \ref org_custusx_filter_cpd

